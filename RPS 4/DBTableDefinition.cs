using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RPS {
    class ColumnInfo {
        //public string name { get; set; }
        public string type { get; set; }
        public bool index { get; set; }
        public bool unique { get; set; }

        public ColumnInfo(string type): this(type, false, false) { }

        public ColumnInfo(string type, bool index): this(type, index, false) { }
        
        public ColumnInfo(string type, bool index, bool unique) {
            //this.name = name;
            this.type = type;
            this.index = index;
            this.unique = unique;
        }
    }

    class DBTableDefinition {
        public string tablename;
        public Dictionary<string, ColumnInfo> columns;

        public DBTableDefinition(string tablename, Dictionary<string, ColumnInfo> columns) {
            this.tablename = tablename;
            this.columns = columns;
        }
        
        public string SQLColumnDefs() {
            string s = null;
            foreach(KeyValuePair<string, ColumnInfo> column in this.columns) {
                if (s != null) s += ",";
                s += "`" + column.Key + "` " + column.Value.type;
            }
            return s;
        }

        public string CreateTableSQL() {
            string sql = "PRAGMA page_size=4096; DROP TABLE IF EXISTS `" + this.tablename + "`; CREATE TABLE `" + this.tablename + "` (";
            sql += this.SQLColumnDefs();
            sql += ");";
            return sql;
        }
    }
}
