using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SQLite;
//using System.Data.SQLite.Linq;
using System.Data.Linq;
using System.Data.Linq.Mapping;

namespace RPS {
    [Table(Name = "Setting")]
    class Setting {
        [Column(Name = "id", IsPrimaryKey = true, IsDbGenerated = true)]
        public long Id { get; set; }

        [Column(Name = "key")]
        public string Key { get; set; }

        [Column(Name = "value")]
        public string Value { get; set; }
    }
}
