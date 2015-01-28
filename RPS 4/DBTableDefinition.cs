using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RPS {
    struct FilterInfo {
        public string name;
        public bool searchable;
        public enum VarType { text=0, rawSQL, numeral, date }
        public int varType;

        public FilterInfo(string name, bool searchable, FilterInfo.VarType type) {
            this.name = name;
            this.searchable = searchable;
            this.varType = (int)type;
        }
    }

    class ColumnInfo {
        //public string name { get; set; }
        public string type { get; set; }
        public FilterInfo filterInfo { get; set; }
        public bool index { get; set; }
        public bool unique { get; set; }
        public bool searchable { get; set; }

        public ColumnInfo(string type) : this(type, new FilterInfo("", false, FilterInfo.VarType.text), false, false) { }

        public ColumnInfo(string type, bool index, bool unique) : this(type, new FilterInfo("", false, FilterInfo.VarType.text), index, unique) { }

        public ColumnInfo(string type, FilterInfo fi) : this(type, fi, false, false) { }

        public ColumnInfo(string type, FilterInfo fi, bool index) : this(type, fi, index, false) { }

        public ColumnInfo(string type, FilterInfo fi, bool index, bool unique) {
            //this.name = name;
            this.type = type;
            this.filterInfo = fi;
            this.index = index;
            this.unique = unique;
            this.searchable = searchable;
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
