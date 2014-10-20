using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RPS {
    public class jsonFolder {
        public bool folder = true;
        public bool selected { get; set; }
        public bool expanded { get; set; }
        public bool unselectable { get; set; }
        public string extraClasses { get; set; }
        public string key { get; set; }
        public string title { get; set; }
        public bool lazy { get; set; } // Has subfolders

        public List<jsonFolder> children;

        public jsonFolder() : this(null, false, new List<jsonFolder>()) { }

        public jsonFolder(string title) : this(title, false, new List<jsonFolder>()) { }

        public jsonFolder(string title, List<jsonFolder> children) : this(title, false, children) { }

        public jsonFolder(string title, bool selected, List<jsonFolder> children) {
            this.expanded = false;
            this.selected = false;
            this.unselectable = false;
            this.key = title.ToLower();
            this.title = title;
            this.selected = selected;
            this.children = children;
        }

        public jsonFolder hasChild(string key) {
            for (int i = 0; i < this.children.Count; i++) {
                if (this.children[i].key == key) return this.children[i];
            }
            return null;
        }
    }
}
