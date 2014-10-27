using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace RPS {
    class MetadataTemplate {
  //      string rxGroups = "<#[^#]*#>";
//        string rxTags = "{[^}]*}"
        public string template { get; set; }
        Dictionary<string, string> metadata;

        public MetadataTemplate(string metadata):this(metadata, "") { }

        public MetadataTemplate(string metadata, string template) {
            this.template = template;
            this.parseMetadata(metadata);
        }

        public void parseMetadata(string metaString) {
            this.metadata = new Dictionary<string, string>();

            if (metaString != null) foreach (string s in metaString.Split(new[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries)) {
                string[] kv = s.Split('\t');
                if (kv.Length == 2)
                    this.metadata[kv[0].ToLower()] = kv[1];

                //Debug.Assert(kv.Length == 2, "Can not parse line :'" + s + "'");
            }
/*
            this.metadata = s.Split(new[] {"\r\n"}, StringSplitOptions.RemoveEmptyEntries)
                            .Select(part => part.Split(new[] { "\t" }, StringSplitOptions.None))
                            .ToDictionary(split => split[0], split => split[1]);*/
        }

        public Dictionary<string, string> filter(string keyFilter) {
            Dictionary<string, string> md = new Dictionary<string, string>();

            return md;
        }

        public string fillTemplate() {
            return this.fillTemplate(null);
        }

        public string fillTemplate(string template) {
            string output = "";
            if (template != null) this.template = template;
            Debug.WriteLine(this.template);
            if (this.metadata.Count > 0) {
                int pos = 0;
                Regex rxGroups = new Regex("<#[^#]*#>", RegexOptions.Compiled);
                Regex rxTags = new Regex("{[^}]*}", RegexOptions.Compiled);

                MatchCollection groupMatches = rxGroups.Matches(this.template);
                foreach (Match groupMatch in groupMatches) {
                    string tagCode = "";
                    string metaKey = "";

                    output += this.template.Substring(pos, groupMatch.Index - pos);

                    MatchCollection tagMatches = rxTags.Matches(groupMatch.Value);
                    // Start at position 2 to strip <#
                    int tagPos = 2;
                    foreach (Match tagMatch in tagMatches) {
                        tagCode += groupMatch.Value.Substring(tagPos, tagMatch.Index - tagPos);
                        metaKey = tagMatch.Value.ToLower();
                        tagCode += metaKey;
                        metaKey = metaKey.Trim(new char[] { '{', '}' });
                        //Debug.WriteLine(tagMatch.Value);
                        //tagCode += metaKey;
                        tagPos = tagMatch.Index + tagMatch.Length;
                    }
                    tagCode += groupMatch.Value.Substring(tagPos, groupMatch.Value.Length-tagPos-2);
                    
                    // Get keys, 
                    // perform match on keys
                    // loop 

                    if (this.metadata.ContainsKey(metaKey)) {
                        output += tagCode.Replace("{" + metaKey + "}", this.metadata[metaKey]);
                    }
                    pos = groupMatch.Index + groupMatch.Length;
                }
                output += this.template.Substring(pos);
            }
            return output;
        }

    }
}
