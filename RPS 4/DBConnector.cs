using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Data.SQLite;
using System.IO;
using System.Data;
using System.Diagnostics;

/**
 * Run time memory based SQLite DB connection that is automatically stored in file db.
 **/

namespace RPS {
    public class SortOrder {
        public enum Direction { ASC, DESC };
        public Direction direction;

        public SortOrder(Direction direction) {
            this.direction = direction;
        }

        public SortOrder(String direction) {
            if (direction.ToUpper() == "DESC") this.direction = Direction.DESC;
            else this.direction = Direction.ASC;
        }

        public bool isASC() {
            return (this.direction == Direction.ASC);
        }

        public bool isDESC() {
            return (this.direction == Direction.DESC);
        }

        public void setASC() {
            this.direction = Direction.ASC;
        }

        public void setDESC() {
            this.direction = Direction.DESC;
        }

        public SortOrder toggle() {
            if (this.direction == Direction.ASC) return new SortOrder(Direction.DESC);
            else return new SortOrder(Direction.ASC);
        }

        public string ToString() {
            if (this.direction == Direction.DESC) return "DESC";
            else return "ASC";
        }
    }

    class DBConnector {
        private string filename;
        private string CreateTableSQL;
        public SQLiteConnection connection;
        private SQLiteCommand command;
        private bool keepInMemory;


        public DBConnector(string filename, string CreateTableSQL) :this(filename, CreateTableSQL, true) { }

        public DBConnector(string filename, string CreateTableSQL, bool keepInMemory) {
            this.filename = filename;
            this.CreateTableSQL = CreateTableSQL;
            this.keepInMemory = keepInMemory;
            if (this.keepInMemory) {
                this.connection = new SQLiteConnection("Data Source=:memory:;Version=3;");
                this.connection.Open();
                this.loadDBFromFile();
            } else {
                this.connection = this.openCreateFileDB(filename);
                //this.connection.Open();
            }
        }
        
        public static string ToReadableString(SQLiteCommand command) {
            StringBuilder builder = new StringBuilder();
            string sql = command.CommandText;
            builder.AppendLine("");
            if (command.Parameters.Count > 0)
                builder.AppendLine("With the following parameters.");

            foreach (SQLiteParameter param in command.Parameters) {
                sql = sql.Replace(param.ParameterName, param.Value.ToString());
                builder.AppendFormat(
                    "     Paramater {0}: {1}",
                    param.ParameterName,
                    (param.Value == null ?
                    "NULL" : param.Value.ToString())).AppendLine();
            }
            builder.Insert(0, "Sql command: " + sql);
            return builder.ToString();
        }

        protected SQLiteConnection openCreateFileDB() {
            return this.openCreateFileDB(null);
        }

        protected SQLiteConnection openCreateFileDB(string filename) {
            SQLiteConnection filePersists;
            if (filename == null) filename = this.filename;
            if (!File.Exists(filename)) {
                if (!Directory.Exists(Path.GetDirectoryName(filename))) {
                    Directory.CreateDirectory(Path.GetDirectoryName(filename));
                }
                SQLiteConnection.CreateFile(filename);
                filePersists = new SQLiteConnection("Data Source=" + filename + ";Version=3;");
                filePersists.Open();
                SQLiteCommand sqlCommand = new SQLiteCommand(this.CreateTableSQL, filePersists);
                sqlCommand.ExecuteNonQuery();
            } else {
                filePersists = new SQLiteConnection("Data Source=" + filename + ";Version=3;");
                filePersists.Open();
            }
            return filePersists;
        }

        protected void loadDBFromFile() {
            if (this.keepInMemory) {
                SQLiteConnection filePersists = this.openCreateFileDB();
                filePersists.BackupDatabase(this.connection, "main", "main", -1, null, 0);
                filePersists.Close();
            }
        }

        public void saveDBToFile() {
            if (this.keepInMemory) {
                SQLiteConnection filePersists = this.openCreateFileDB();
                this.connection.BackupDatabase(filePersists, "main", "main", -1, null, 0);
                filePersists.Close();
            }
        }

        public SQLiteDataReader ExecuteReader(string query) {
            SQLiteCommand sqlCommand = new SQLiteCommand(query, this.connection);
            SQLiteDataReader sqlReader = sqlCommand.ExecuteReader();
            return sqlReader;
        }

        public SQLiteCommand ExecuteNonQuery(string query) {
            SQLiteCommand sqlCommand = new SQLiteCommand(query, this.connection);
            sqlCommand.ExecuteNonQuery();
            return sqlCommand;
        }

        public void Close() {
            this.connection.Close();
        }

        public static DataRow executeReaderFirstDataRow(SQLiteCommand command) {
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            SQLiteDataReader reader = command.ExecuteReader();
            DataTable dt = new DataTable();
            dt.Load(reader);
            sw.Stop();
            Debug.WriteLine(DBConnector.ToReadableString(command) + " in " + sw.ElapsedMilliseconds + "ms");
            if (dt.Rows.Count >= 1) {
                return dt.Rows[0];
            } else {
                return null;
            }
        }
    }
}
