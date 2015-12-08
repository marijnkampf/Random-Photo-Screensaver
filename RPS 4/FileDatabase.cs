using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Data.SQLite;
using System.IO;
using System.Security.Permissions;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Collections.Concurrent;
//using System.Configuration;

namespace RPS {
    class FileDatabase {
        DBConnector dbConnector;
        DBConnector metaDataDbConnector;
        SQLiteTransaction metaDataTransaction;
        int filterOutOfDate = 0;
        bool useFilter = false;
        public bool readOnly = false;
        string filterSQL = null;

        public FileDatabase(bool readOnly) {
            this.readOnly = readOnly;
            this.dbConnector = new DBConnector(Constants.selectProgramAppDataFolder(Constants.DbFileName), Constants.FileNodesDefinition);

            // Connect to metadata regardless so it can be switched on whilst running
            string mdbPath = Constants.selectProgramAppDataFolder(Constants.MetadataFileName);
            this.metaDataDbConnector = new DBConnector(
                mdbPath, 
                Constants.MetadataDefinition, 
                false
            );

            this.dbConnector.ExecuteNonQuery(@"ATTACH DATABASE '" + mdbPath + "' AS mdb;");
        }

        public void CloseConnections() {
            this.dbConnector.Close();
            this.toggleMetadataTransaction(true);
            this.metaDataDbConnector.Close();
        }

        public void setFilterSQL(string sql) {
            this.filterSQL = sql.Replace("\\t", "\t").Replace("\\n", "\n");
            this.useFilter = true;
            this.filterReady(true);
        }

        public void resetFilter() {
            this.filterReady(true);
        }

        public void resetIfChangedFilter() {
            if (this.filterOutOfDate > 0) this.filterReady(true);
        }

        public void clearFilter() {
            this.useFilter = false;
        }
        public string filterReady() {
            return this.filterReady(false);
        }

        public string filterReady(bool force) {
            if (!this.useFilter) return "FileNodes";
            if (this.useFilter && (this.filterOutOfDate >= Constants.reloadFilters || force)) {
                SQLiteCommand command;
                System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
                sw.Start();
                // Potential optimisation: Only use filter if Metadata used?
                command = new SQLiteCommand("DROP TABLE IF EXISTS Filter;", this.dbConnector.connection);
                command.ExecuteNonQuery();
                if (this.filterSQL == null) {
                    command = new SQLiteCommand("CREATE TEMP TABLE Filter AS SELECT * FROM `FileNodes` LEFT JOIN mdb.Metadata USING (id)", this.dbConnector.connection);
                } else {
                    command = new SQLiteCommand("CREATE TEMP TABLE Filter AS SELECT * FROM `FileNodes` LEFT JOIN mdb.Metadata USING (id) WHERE " + this.filterSQL, this.dbConnector.connection);
                }
                command.ExecuteNonQuery();
                sw.Stop();
                //Debug.WriteLine("Applying filter: " + DBConnector.ToReadableString(command) + sw.ElapsedMilliseconds + "ms");

                this.filterOutOfDate = 0;
            }
            return "Filter";
        }

        public DataTable runSQLByPassFilter(string subQuery) {
            SQLiteCommand sqlCommand = new SQLiteCommand("SELECT * FROM `FileNodes` LEFT JOIN mdb.Metadata USING (id) WHERE " + subQuery, this.dbConnector.connection);
            SQLiteDataReader sqlReader = sqlCommand.ExecuteReader();
            DataTable dt = new DataTable();
            dt.Load(sqlReader);
            return dt;
        }

        public void addFileToDB(FileInfo fi) {
            /****
             * 	New DB	Existing DB
             * 	One statement                                   (new DB)    (existing data)
             * 	        INSERT REPLACE	                        929ms	    1013ms
             * 	is slower then tw
             * 	        SELECT => nothing, INSERT or UPDATE	    570ms	    549ms
             * 	        
             * Tested single line statement: SQLiteCommand insertSQL = new SQLiteCommand("INSERT OR IGNORE INTO FileNodes (path, parentpath, created, modified, size) VALUES (@path, @pathHash, @parentPathHash, @created, @modified, @size) ", this.dbConnector.connection);
             *****/
            if (this.readOnly) return;

            SQLiteCommand command;
            command = new SQLiteCommand("SELECT id, created, modified, size FROM `FileNodes` WHERE path = @path LIMIT 1 COLLATE NOCASE;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@path", fi.FullName);
            SQLiteDataReader reader = command.ExecuteReader();
            DataTable dt = new DataTable();
            //try {
            dt.Load(reader);
            //} catch (System.InvalidOperationException ioe) {
                //MessageBox.Show("addFileToDB " + e.Message);
                // ToDo debug and work out exceptions that can occur here
              //  return;
            //}
            bool executeNonQuery = false;
            if (dt.Rows.Count > 0) {
                if ((DateTime)dt.Rows[0]["created"] < fi.CreationTime ||
                    (DateTime)dt.Rows[0]["modified"] < fi.LastWriteTime ||
                    (long)dt.Rows[0]["size"] != fi.Length) 
                {
                    command = new SQLiteCommand("UPDATE `FileNodes` SET `created` = @created, `filename` = @filename, `modified` = @modified, `size` = @size, `metainfoindexed` = 0 WHERE `id` = @id", this.dbConnector.connection);
                    command.Parameters.AddWithValue("@id", (long)dt.Rows[0]["id"]);
                    executeNonQuery = true;
                }
            } else {
                command = new SQLiteCommand("INSERT INTO FileNodes (`path`, `parentpath`, `filename`, `created`, `modified`, `size`) " +
                                                           "VALUES (@path,  @parentpath,  @filename,  @created,  @modified,  @size) ", this.dbConnector.connection);
                command.Parameters.AddWithValue("@path", fi.FullName);
                // Add trailing slash so we can filter on complete paths ('c:\test\' won't match 'c:\testing\')
                command.Parameters.AddWithValue("@parentpath", Utils.addTrailingSlash(fi.DirectoryName));
                executeNonQuery = true;
            }
            if (executeNonQuery) { 
                command.Parameters.AddWithValue("@created", fi.CreationTime);
                command.Parameters.AddWithValue("@modified", fi.LastWriteTime);
                command.Parameters.AddWithValue("@filename", fi.Name);
                command.Parameters.AddWithValue("@size", fi.Length);
                System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
                sw.Start();
                command.ExecuteNonQuery();
                sw.Stop();
                //Debug.WriteLine(DBConnector.ToReadableString(command) + " " + sw.ElapsedMilliseconds + "ms");
                this.filterOutOfDate++;
            }
        }

        public DataRow getFirstImage(string orderBy, SortOrder direction) {
            return getFirstImage(orderBy, direction, 0);
        }

        public DataRow getFirstImage(string orderBy, SortOrder direction, long offset) {
            string tableName = this.filterReady();
            SQLiteCommand command;
            if (orderBy == null) orderBy = "created";
            if (direction == null) direction = new SortOrder(SortOrder.Direction.ASC);
            command = new SQLiteCommand("SELECT * FROM `" + tableName + "` ORDER BY " + orderBy + " " + direction.ToString() + ", id " + direction.ToString() + " LIMIT 1 OFFSET " + Math.Abs(offset) + ";", this.dbConnector.connection);
            command.Parameters.AddWithValue("@orderBy", orderBy);
            return DBConnector.executeReaderFirstDataRow(command);
        }

        public DataRow getImageById(long id, long offset) {
            return this.getImageById(id, offset, null, null);
        }

        public DataRow getImageById(long id, long offset, string sortByColumn, SortOrder sortByDirection) {
            string tableName = this.filterReady();
            SQLiteCommand command;
            DataRow dr;
            if (offset == 0) {
                command = new SQLiteCommand("SELECT * FROM `" + tableName + "` WHERE (id = @id) LIMIT 1;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                dr = DBConnector.executeReaderFirstDataRow(command);
                if (dr == null) {
                    return this.getFirstImage(sortByColumn, sortByDirection);
                }
            } else {
                if (sortByColumn == null) sortByColumn = "path";
                if (sortByDirection == null) sortByDirection = new SortOrder(SortOrder.Direction.ASC);
                string than = "<";

                if (sortByDirection.isDESC()) offset *= -1;
                if (offset < 0) {
                    than = "<";
                    sortByDirection.setDESC();
                } else {
                    than = ">";
                    sortByDirection.setASC();
                }

                string sqlValue = "(SELECT " + sortByColumn + " FROM `" + tableName + "` WHERE `" + tableName + "`.id = @id)";

                string sql = "FROM `" + tableName + "` WHERE (" + sortByColumn + " " + than + " " + sqlValue + ") " +
                             " OR ((" + sortByColumn + " = " + sqlValue + " AND id " + than + " @id)) " +
                             "ORDER BY " + sortByColumn + " " + sortByDirection.ToString() + ", id " + sortByDirection.ToString();

                command = new SQLiteCommand(@"SELECT * " + sql + " LIMIT 1 OFFSET " + (Math.Abs(offset)-1) + ";", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                dr = DBConnector.executeReaderFirstDataRow(command);
                if (dr == null) {
                    command = new SQLiteCommand(@"SELECT COUNT(`" + tableName + "`.id) " + sql, this.dbConnector.connection);
                    command.Parameters.AddWithValue("@id", id);
                    dr = DBConnector.executeReaderFirstDataRow(command);
                    if (dr != null) {
                        if (offset < 0) offset += Convert.ToInt32(dr[0])+1;
                        else offset -= Convert.ToInt32(dr[0])+1;
                        if (this.nrImagesFilter() > 0) {
                            offset = offset % this.nrImagesFilter();
                        }
                        dr = getFirstImage(sortByColumn, sortByDirection, offset);
                    }
                }
            }
            return dr;
        }

        public DataRow getRandomImage(long offset) {
            /***
             *  Single query "SELECT * FROM `Filter` ORDER BY RANDOM() LIMIT 1;" takes approximately 1000ms 
             *  Two separate queries take approximately 100ms (combined)
             *  
             * TODO optimise?
             * Test Select "SELECT id, path, ..., ... FROM `Filter` ORDER BY RANDOM() LIMIT 1;" ~ 450ms - 500ms
             ***/
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT id FROM `" + tableName + "` ORDER BY RANDOM() LIMIT 1;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr == null) return null;
            return getImageById(Convert.ToInt32(dr["id"]), offset);
        }

        public long getIdFromPath(string path) {
            SQLiteCommand command = new SQLiteCommand("SELECT id FROM `FileNodes` WHERE path = @path;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@path", path);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr == null) return 0;
            return Convert.ToInt32(dr["id"]);
        }

        public int deleteFromDB(string path) {
            long id = getIdFromPath(path);
            if (id == 0) return -1;
            return deleteFromDB(id);
        }

        public int deleteFromDB(long id) {
            if (this.readOnly) return -1;
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("DELETE FROM `FileNodes` WHERE id = @id;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            int r = command.ExecuteNonQuery();
            command = new SQLiteCommand("DELETE FROM `Metadata` WHERE id = @id;", this.metaDataDbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            try { 
                r += command.ExecuteNonQuery();
            } catch (Exception e) {
                Debug.WriteLine("Error on deleteFromDB " + e.Message);
            }
            return r;
        }

        public int purgeMediaDatabase() {
            if (this.readOnly) return -1;

            SQLiteCommand command;
            int r = 0;
            try {
                command = new SQLiteCommand("DELETE FROM `FileNodes`; VACUUM;", this.dbConnector.connection);
                command.ExecuteNonQuery();
                command = new SQLiteCommand("DELETE FROM `Metadata`; VACUUM;", this.metaDataDbConnector.connection);
                command.ExecuteNonQuery();
            } catch (Exception e) {
                Debug.WriteLine("Failed to empty media database " + e.Message);
            }
            return r;
        }

        public int purgeMetadata() {
            if (this.readOnly) return -1;

            //string where = "WHERE NOT parentpath LIKE \"" + String.Join("%\" AND NOT parentpath LIKE \"", folders) + "%\"";
            SQLiteCommand command;
            int r = 0;
            try {
                command = new SQLiteCommand("SELECT id FROM mdb.Metadata WHERE mdb.Metadata.id in (SELECT mdb.Metadata.id FROM mdb.Metadata LEFT JOIN `FileNodes` ON mdb.Metadata.id = `FileNodes`.`id` WHERE `FileNodes`.`id` IS NULL);", this.dbConnector.connection);
                //command = new SQLiteCommand("DELETE FROM mdb.Metadata WHERE mdb.Metadata.id in (SELECT `FileNodes`.`id` FROM `FileNodes` LEFT JOIN mdb.Metadata ON mdb.Metadata.id = FileNodes.id WHERE mdb.Metadata.id IS NULL);", this.dbConnector.connection);
                //r = command.ExecuteNonQuery();
                SQLiteDataReader reader = command.ExecuteReader();
                DataTable dt = new DataTable();
                dt.Load(reader);
                foreach (DataRow row in dt.Rows) {

                    command = new SQLiteCommand("DELETE FROM `Metadata` WHERE id = @id;", this.metaDataDbConnector.connection);
                    command.Parameters.AddWithValue("@id", row["id"]);
                    r += command.ExecuteNonQuery();
                    if (r % 200 == 0) this.toggleMetadataTransaction();
                }
                this.toggleMetadataTransaction();
            } catch (Exception e) {
                Debug.WriteLine("purgeMetadata " + e.Message);
            }
            return r;
        }

        public int purgeNotMatchingParentFolders(List<string> folders, bool exactMatchFolders, List<string> excludedSubfolders) {
            if (this.readOnly) return -1;

            string match = "%";
            if (exactMatchFolders) match = "";
            string where = "WHERE NOT parentpath LIKE \"" + String.Join(match + "\" AND NOT parentpath LIKE \"", folders) + match + "\"";
            if (excludedSubfolders.Count > 0) {
                where += " OR parentpath LIKE \"%\\" + String.Join("\\%\" OR parentpath LIKE \"%\\", excludedSubfolders) + "\\%\"";
            }
            SQLiteCommand command;
            int r = 0;

            command = new SQLiteCommand("DELETE FROM `FileNodes` " + where + ";", this.dbConnector.connection);
            Debug.WriteLine(command.CommandText);
            try {
                r += command.ExecuteNonQuery();
            } catch (Exception e) {
                return -1;
            }
            return r;
        }

        public int purgeMatchingParentPaths(string fullpath) {
            if (this.readOnly) return -1;

            //Debug.WriteLine(this.nrImagesInDB());

            string where = "WHERE parentpath LIKE \"" + fullpath + "%\"";
            SQLiteCommand command;
            int r = 0;
            command = new SQLiteCommand("DELETE FROM `FileNodes` " + where + ";", this.dbConnector.connection);
            //Debug.WriteLine(command.CommandText);
            try {
                r += command.ExecuteNonQuery();
            } catch (Exception e) {
                return -1;
            }

            //Debug.WriteLine(this.nrImagesInDB());
            
            return r;
        }

        public void storePersistant() {
            if (this.readOnly) return;

            this.dbConnector.saveDBToFile();
        }

        public string getMetadataById(long id) {
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT `all` FROM `Metadata` WHERE id = @id;", this.metaDataDbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr != null && dr.Table.Columns.Contains("all")) {
                return Convert.ToString(dr["all"]);
            } else return null;
        }

        public int nrImagesInDB() {
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT COUNT(id) FROM `FileNodes`;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr != null) return Convert.ToInt32(dr[0]);
            else return -1;
        }

        public int nrImagesFilter() {
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT COUNT(id) FROM `" + tableName + "`;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr != null) return Convert.ToInt32(dr[0]);
            else return -1;
        }

        public int nrMetadataImagesToProcess() {
            SQLiteCommand command = new SQLiteCommand("SELECT COUNT(id) FROM `FileNodes` WHERE metainfoindexed = 0 LIMIT 1;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr != null) return Convert.ToInt32(dr[0]);
            else return -1;
        }

        public DataRow nextMetadataLessImage() {
            SQLiteCommand command = new SQLiteCommand("SELECT * FROM `FileNodes` WHERE metainfoindexed = 0 LIMIT 1;", this.dbConnector.connection);
            return DBConnector.executeReaderFirstDataRow(command);
        }

        public void toggleMetadataTransaction() {
            this.toggleMetadataTransaction(false);
        }

        public void toggleMetadataTransaction(bool closeOnly) {
            try {
                if (!this.readOnly && this.metaDataTransaction != null && this.metaDataTransaction.Connection != null) {
                    this.metaDataTransaction.Commit();
                }
                if (!closeOnly) this.metaDataTransaction = this.metaDataDbConnector.connection.BeginTransaction(true);
            } catch (System.Data.SQLite.SQLiteException se) {
                if (se.ErrorCode == DBConnector.DatabaseIsLocked) {
                    this.readOnly = true;
                }
            }
        }

        public bool updateFileNodesPath(long id, string path) {
            if (this.readOnly) return false;
            try {
                SQLiteCommand command = new SQLiteCommand("UPDATE `FileNodes` SET `path` = @path, `filename` = @filename WHERE id = @id;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                command.Parameters.AddWithValue("@path", path);
                command.Parameters.AddWithValue("@filename", Path.GetFileName(path));
                command.ExecuteNonQuery();
                this.filterOutOfDate+=5;
            } catch (System.Data.SQLite.SQLiteException se) {
                if (se.ErrorCode == DBConnector.DatabaseIsLocked) {
                    this.readOnly = true;
                    return false;
                }
            }
            return true;
        }

        public bool renameFolderPaths(string oldPath, string newPath) {
            if (this.readOnly) return false;
            try {
                SQLiteCommand command = new SQLiteCommand("UPDATE `FileNodes` SET `path` = replace(`path`, @oldPath, @newPath), `parentpath` = replace(`parentpath`, @oldPath, @newPath)  WHERE `path` LIKE @oldPath;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@oldPath", oldPath);
                command.Parameters.AddWithValue("@newPath", newPath);
                command.ExecuteNonQuery();
                this.filterOutOfDate += 5;
            } catch (System.Data.SQLite.SQLiteException se) {
                if (se.ErrorCode == DBConnector.DatabaseIsLocked) {
                    this.readOnly = true;
                    return false;
                }
            }
            return true;
        }

        public bool addMetadataToDB(long id, string metadata) {
            if (this.readOnly) return false;
            try {
                long width=0, height=0, area=0;
                MetadataTemplate mt = new MetadataTemplate(metadata);
                if (mt.metadata.ContainsKey("imagewidth")) width = Convert.ToInt32(mt.metadata["imagewidth"]);
                if (mt.metadata.ContainsKey("imageheight")) height = Convert.ToInt32(mt.metadata["imageheight"]);
                area = width * height;
                SQLiteCommand command = new SQLiteCommand("INSERT OR REPLACE INTO `Metadata` (`id`, `all`, `width`, `height`, `area`) VALUES (@id, @metadata, @width, @height, @area);", this.metaDataDbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                command.Parameters.AddWithValue("@metadata", metadata);
                command.Parameters.AddWithValue("@width", width);
                command.Parameters.AddWithValue("@height", height);
                command.Parameters.AddWithValue("@area", area);
                command.ExecuteNonQuery();
            
                command = new SQLiteCommand("UPDATE `FileNodes` SET `metainfoindexed` = 1 WHERE id = @id;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                command.ExecuteNonQuery();
                this.filterOutOfDate+=5;
            } catch (System.Data.SQLite.SQLiteException se) {
                if (se.ErrorCode == DBConnector.DatabaseIsLocked) {
                    this.readOnly = true;
                    return false;
                }
            } catch (System.FormatException fe) {
                return true;
            }
            return true;
        }
    }
}
