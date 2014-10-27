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
/*
    public delegate void MetadataReadEventHandler(ImageMetadataStatus eventArgs);

    public class ImageMetadataStatus {
        public enum Processing { Queued, Processing, Completed };
        public long monitorId;
        public DataRow image;
        public string metadata;
        public Processing processing;
        public ImageMetadataStatus(long monitorId, DataRow image) {
            this.processing = Processing.Queued;
            this.monitorId = monitorId;
            this.image = image;
        }
    }

    public class MetadataReadEventArgs : EventArgs {
        public MetadataReadEventArgs(ImageMetadataStatus status) {
            this.status = status;
        }
        public ImageMetadataStatus status { get; set; }
    }*/

    class FileDatabase {
        DBConnector dbConnector;
        DBConnector metaDataDbConnector;
        SQLiteTransaction metaDataTransaction;
        int filterOutOfDate = 0;
        bool useFilter = false;
        string filterSQL = null;
        //private Screensaver screensaver;

        //public FileDatabase(): this(null) {}

        /*
        List<ImageMetadataStatus> monitorMetadataQueueue;
        public event MetadataReadEventHandler MetadataReadEvent;
         */

        public FileDatabase(/*Screensaver screensaver*/) {
            //this.monitorMetadataQueueue = new List<ImageMetadataStatus>();
            //this.screensaver = screensaver;
            this.dbConnector = new DBConnector(
                Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), 
                    Constants.AppFolderName,
                    Constants.DbFileName
                ),
                Constants.FileDatabaseSQL
            );

            // Connect to metadata regardless so it can be switched on whilst running
            string mdbPath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                Constants.AppFolderName,
                Constants.MetadataFileName
            );
            this.metaDataDbConnector = new DBConnector(
                mdbPath, 
                Constants.MetadataSQL, 
                false
            );

            this.dbConnector.ExecuteNonQuery(@"ATTACH DATABASE '" + mdbPath + "' AS mdb;");
            
            /*
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            SQLiteCommand command = new SQLiteCommand("CREATE TEMP TABLE Filter AS SELECT * FROM `FileNodes` NATURAL JOIN mdb.Metadata WHERE `all` LIKE \"%Exposure Time	1/100\" || x'0d' || x'0a' || \"%\"", this.dbConnector.connection);
            command.ExecuteNonQuery();
            command = new SQLiteCommand("SELECT * FROM `Filter` ORDER BY RANDOM() LIMIT 1", this.dbConnector.connection);
            SQLiteDataReader reader = command.ExecuteReader();
            DataTable dt = new DataTable();
            dt.Load(reader);
            sw.Stop();
            Debug.WriteLine(sw.ElapsedMilliseconds + "ms");
            */
            /*
             System.Environment.SpecialFolder.LocalApplicationData
             CommonPictures
             CommonVideos
             ProgramFiles
             MyPictures
             MyVideos
             System
                */
        }

        public void setFilterSQL(string sql) {
            this.filterSQL = sql;
            //this.filterOutOfDate = Constants.reloadFilters;
            this.useFilter = true;
            this.filterReady(true);
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
                    command = new SQLiteCommand("CREATE TEMP TABLE Filter AS SELECT * FROM `FileNodes` NATURAL JOIN mdb.Metadata", this.dbConnector.connection);
                } else {
                    command = new SQLiteCommand("CREATE TEMP TABLE Filter AS SELECT * FROM `FileNodes` NATURAL JOIN mdb.Metadata WHERE " + this.filterSQL, this.dbConnector.connection);
                }
                command.ExecuteNonQuery();
                sw.Stop();
                Debug.WriteLine("Applying filter: " + DBConnector.ToReadableString(command) + sw.ElapsedMilliseconds + "ms");

                this.filterOutOfDate = 0;
            }
            return "Filter";
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
            SQLiteCommand command;
            command = new SQLiteCommand("SELECT id, path, parentpath, created, modified, size FROM `FileNodes` WHERE path = @path LIMIT 1;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@path", fi.FullName);
            SQLiteDataReader reader = command.ExecuteReader();
            DataTable dt = new DataTable();
            dt.Load(reader);
            bool executeNonQuery = false;
            if (dt.Rows.Count > 0) {
                if ((DateTime)dt.Rows[0]["created"] != fi.CreationTime ||
                    (DateTime)dt.Rows[0]["modified"] != fi.LastWriteTime ||
                    (long)dt.Rows[0]["size"] != fi.Length) 
                {
                    command = new SQLiteCommand("UPDATE `FileNodes` SET `created` = @created, `modified` = @modified, `size` = @size, `metainfoindexed` = 0 WHERE `id` = @id", this.dbConnector.connection);
                    command.Parameters.AddWithValue("@id", (long)dt.Rows[0]["id"]);
                    executeNonQuery = true;
                }
            } else {
                command = new SQLiteCommand("INSERT INTO FileNodes (`path`, `parentpath`, `created`, `modified`, `size`) VALUES (@path, @parentpath, @created, @modified, @size) ", this.dbConnector.connection);
                command.Parameters.AddWithValue("@path", fi.FullName);
                command.Parameters.AddWithValue("@parentpath", fi.DirectoryName);
                executeNonQuery = true;
            }
            if (executeNonQuery) { 
                command.Parameters.AddWithValue("@created", fi.CreationTime);
                command.Parameters.AddWithValue("@modified", fi.LastWriteTime);
                command.Parameters.AddWithValue("@size", fi.Length);
                System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
                sw.Start();
                command.ExecuteNonQuery();
                sw.Stop();
                Debug.WriteLine(DBConnector.ToReadableString(command) + " " + sw.ElapsedMilliseconds + "ms");
                this.filterOutOfDate++;
            }
        }

        public DataRow getFirstImage(long id, string orderBy, SortOrder direction) {
            return getFirstImage(id, orderBy, direction, 0);
        }

        public DataRow getFirstImage(long id, string orderBy, SortOrder direction, long offset) {
            string tableName = this.filterReady();
            SQLiteCommand command;
            if (id > 0) {
                command = new SQLiteCommand("SELECT * FROM `" + tableName + "` WHERE `FileNodes`.id = @id LIMIT 1;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
            } else {
                if (orderBy == null) orderBy = "created";
                command = new SQLiteCommand("SELECT * FROM `" + tableName + "` ORDER BY " + orderBy + " " + direction.ToString() + " LIMIT 1 OFFSET " + offset + ";", this.dbConnector.connection);
                command.Parameters.AddWithValue("@orderBy", orderBy);
             //   command.Parameters.AddWithValue("@direction", direction);
            }
            return DBConnector.executeReaderFirstDataRow(command);
        }

        public DataRow getLastImage(string orderBy, SortOrder direction) {
            // Reverse sort direction and get first image
            direction.toggle();
            return getFirstImage(-1, orderBy, direction);
        }

        public DataRow getImageById(long id, long offset) {
            return this.getImageById(id, offset, new SortOrder(SortOrder.Direction.ASC), null, null);
        }

        public DataRow getImageById(long id, long offset, SortOrder direction, string sortBy, SortOrder orderingTerm) {
            string tableName = this.filterReady();
            SQLiteCommand command;
            DataRow dr;
            if (offset == 0) {
                command = new SQLiteCommand("SELECT * FROM `" + tableName + "` WHERE (id = @id) LIMIT 1;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                dr = DBConnector.executeReaderFirstDataRow(command);
                if (dr == null) {
                    //throw new Exception("id '"+id+'" not found. ");
                    return this.getFirstImage(-1, sortBy, direction);
                }
            } else {
                if (sortBy == null) sortBy = "path";
                //if (orderingTerm == null) orderingTerm = "ASC";
                string than = "";
                if (orderingTerm.isDESC()) {
                    than = "<";
                    if (direction.isDESC()) {
                        than = ">";
                        orderingTerm.toggle();
                    }
                } else {
                    than = ">";
                    if (direction.isDESC()) {
                        than = "<";
                        orderingTerm.toggle();
                    }
                }

                string sql = "FROM `" + tableName + "` WHERE " + sortBy + " " + than + " (SELECT " + sortBy + " FROM `" + tableName + "` WHERE `FileNodes`.id = @id) ORDER BY " + sortBy + " " + orderingTerm.ToString();
                
                command = new SQLiteCommand(@"SELECT * " + sql + " LIMIT 1 OFFSET " + (offset - 1) + ";", this.dbConnector.connection);
              //command = new SQLiteCommand(@"SELECT * FROM `FileNodes` WHERE " + than + " (SELECT path FROM `FileNodes` WHERE id = @id) ORDER BY @sortBy DESC LIMIT 1 OFFSET @offset;", this.dbConnector.connection);
                command.Parameters.AddWithValue("@id", id);
                command.Parameters.AddWithValue("@sortBy", sortBy);
                command.Parameters.AddWithValue("@offset", Math.Abs(offset)-1);
                //command = new SQLiteCommand(@"SELECT * FROM `FileNodes` WHERE path > (SELECT path FROM `FileNodes` WHERE id = 4) ORDER BY path ASC LIMIT 1 OFFSET 0;", this.dbConnector.connection);
                dr = DBConnector.executeReaderFirstDataRow(command);
                if (dr == null) {
                    command = new SQLiteCommand(@"SELECT COUNT(`FileNodes`.id) " + sql, this.dbConnector.connection);
                    command.Parameters.AddWithValue("@id", id);
                    dr = DBConnector.executeReaderFirstDataRow(command);
                    if (dr != null) offset -= Convert.ToInt32(dr[0]);
                    dr = getFirstImage(-1, sortBy, orderingTerm, offset - 1);
                }
            }
            return dr;
        }

        public DataRow getRandomImage() {
            /***
             *  Single query "SELECT * FROM `Filter` ORDER BY RANDOM() LIMIT 1;" takes approximately 1000ms 
             *  Two separate queries take approximately 100ms (combined)
             *  
             * 
             * Test Select "SELECT id, path, ..., ... FROM `Filter` ORDER BY RANDOM() LIMIT 1;" ~ 450ms - 500ms
             * 
             * 
             * 
             ***/
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT id FROM `" + tableName + "` ORDER BY RANDOM() LIMIT 1;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            if (dr == null) return null;
            return getImageById(Convert.ToInt32(dr["id"]), 0);
//            SQLiteCommand command = new SQLiteCommand("SELECT * FROM `Filter` ORDER BY RANDOM() LIMIT 1;", this.dbConnector.connection);
  //          DataRow dr = executeReaderFirstDataRow(command);
    //        return dr;

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
            string tableName = this.filterReady();
//            MessageBox.Show("deleting:" + id);
            SQLiteCommand command = new SQLiteCommand("DELETE FROM `FileNodes` WHERE id = @id;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            int r = command.ExecuteNonQuery();
            command = new SQLiteCommand("DELETE FROM `Metadata` WHERE id = @id;", this.metaDataDbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            r += command.ExecuteNonQuery();
            return r;

        }

        public void storePersistant() {
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
            return Convert.ToInt32(dr[0]);
        }

        public int nrImagesFilter() {
            string tableName = this.filterReady();
            SQLiteCommand command = new SQLiteCommand("SELECT COUNT(id) FROM `" + tableName + "`;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            return Convert.ToInt32(dr[0]);
        }

        public int nrMetadataImagesToProcess() {
            SQLiteCommand command = new SQLiteCommand("SELECT COUNT(id) FROM `FileNodes` WHERE metainfoindexed = 0 LIMIT 1;", this.dbConnector.connection);
            DataRow dr = DBConnector.executeReaderFirstDataRow(command);
            return Convert.ToInt32(dr[0]);
        }

        public DataRow nextMetadataLessImage() {
            SQLiteCommand command = new SQLiteCommand("SELECT * FROM `FileNodes` WHERE metainfoindexed = 0 LIMIT 1;", this.dbConnector.connection);
            return DBConnector.executeReaderFirstDataRow(command);
        }

        public void toggleMetadataTransaction() {
            try {
                if (this.metaDataTransaction != null) {
                        this.metaDataTransaction.Commit();
                }
                this.metaDataTransaction = this.metaDataDbConnector.connection.BeginTransaction();
            } catch (System.Data.SQLite.SQLiteException e) {
                // Ignore failed commits;
            }
        }

        public void addMetadataToDB(long id, string metadata) {
            SQLiteCommand command = new SQLiteCommand("INSERT OR REPLACE INTO `Metadata` (`id`, `all`) VALUES (@id, @metadata);", this.metaDataDbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            command.Parameters.AddWithValue("@metadata", metadata);
            command.ExecuteNonQuery();
            
            command = new SQLiteCommand("UPDATE `FileNodes` SET `metainfoindexed` = 1 WHERE id = @id;", this.dbConnector.connection);
            command.Parameters.AddWithValue("@id", id);
            command.ExecuteNonQuery();
            this.filterOutOfDate+=5;
        }
    }
}
