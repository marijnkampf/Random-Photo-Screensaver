if (typeof(window.external.InvokeScriptOnMonitor) === "undefined") {
	if (parent.window.location.href == location.href) {
		redirect = location.href.replace("abdownloads/rps/data/config.html", "random-photo-screensaver/configuration/");
		if (redirect != location.href) location.href = redirect;
	}
}


var NaturalText = {
	"contains": "Containing",
	"begin": "Begining with",
	"ends": "Ending with",
	"equals": "Equals",
	"less": "Less than",
	"greater": "Greater than",
	"currentFilter": "~currentFilter~"
}

// Fix IE 8 issue
NaturalText[null] = "...";

var Filters = function() {
	this.filters = [];
	this.currentFilter = 0;
	this.editorCount = 0;
	this.columns = false;
}

filters = new Filters();

var FilterLine = function (clude, column, match, value, connect) {
	this.clude = clude; // exclude, include
	this.column = column;
	this.match = match; // depends on type
	this.value = value;
	this.connect = connect; // and, or
};

var Filter = function(name, filterLines) {
	this.name = name;
	this.filterLines = filterLines;
}

function initFilters() {
	var data;
	if (typeof(window.external.jsGetFilters)=== "undefined") {
		data = JSON.parse('{"filters":[{"name":"~currentFilter~","filterLines":[{"clude":"include","column":"path","match":"contains","value":"jenni","connect":"and"},{"clude":"exclude","column":"path","match":"contains","value":"marijn","connect":"and"}]},{"name":"Marijn and Jenni","filterLines":[{"clude":"include","column":"path","match":"contains","value":"jenni","connect":"and"},{"clude":"exclude","column":"path","match":"contains","value":"marijn","connect":"and"}]},{"name":"Panoramas","filterLines":[{"clude":"include","column":"filename","match":"contains","value":"pano","connect":"and"}]},{"name":"Small files","filterLines":[{"clude":"include","column":"size","match":"less","value":"100000","connect":"and"}]},{"name":"Wallpaper","filterLines":[{"clude":"include","column":"filename","match":"contains","value":"-","connect":"and"}]}],"currentFilter":0,"editorCount":1,"columns":{"id":{"type":"INTEGER PRIMARY KEY AUTOINCREMENT","filterInfo":{"name":"","searchable":false,"varType":0},"index":true,"unique":true,"searchable":false},"path":{"type":"TEXT UNIQUE","filterInfo":{"name":"paths","searchable":true,"varType":0},"index":true,"unique":true,"searchable":false},"parentpath":{"type":"TEXT","filterInfo":{"name":"parent paths","searchable":true,"varType":0},"index":true,"unique":false,"searchable":false},"filename":{"type":"TEXT","filterInfo":{"name":"filenames","searchable":true,"varType":0},"index":true,"unique":false,"searchable":false},"created":{"type":"DATETIME","filterInfo":{"name":"created date","searchable":true,"varType":2},"index":true,"unique":false,"searchable":false},"modified":{"type":"DATETIME","filterInfo":{"name":"modified date","searchable":true,"varType":2},"index":true,"unique":false,"searchable":false},"size":{"type":"INTEGER","filterInfo":{"name":"size","searchable":true,"varType":1},"index":true,"unique":false,"searchable":false},"metainfoindexed":{"type":"INTEGER DEFAULT 0","filterInfo":{"name":"","searchable":false,"varType":0},"index":false,"unique":false,"searchable":false},"all":{"type":"TEXT","filterInfo":{"name":"metadata","searchable":true,"varType":0},"index":false,"unique":false,"searchable":false},"width":{"type":"TEXT","filterInfo":{"name":"width","searchable":true,"varType":1},"index":false,"unique":false,"searchable":false},"height":{"type":"TEXT","filterInfo":{"name":"height","searchable":true,"varType":1},"index":false,"unique":false,"searchable":false},"area":{"type":"DATETIME","filterInfo":{"name":"area","searchable":true,"varType":1},"index":false,"unique":false,"searchable":false}}}');
	} else {
		//prompt("json", window.external.jsGetFilters());
		try {
			data = JSON.parse(window.external.jsGetFilters());
		} catch(e) {
		}
	}
	// Create filter object from data
	try {
		filters.filters = data.filters;
		filters.edit(NaturalText.currentFilter);
		filters.showFilterList();
	} catch(e) {
	}
}

FilterLine.prototype.getEditorValues = function(id) {
	this.clude = $("#clude_" + id).first().val();
	this.column = $("#column_" + id).val();

	if ($("#match_text_" + id).is(':visible')) {
		this.match = $("#match_text_" + id).val();
	} else {
		this.match = $("#match_numeral_" + id).val();
	}

	this.value = $("#value_" + id).val();
	if ($("#connect_and_" + id).prop('checked')) this.connect = "and";
	else this.connect = "or";
	return this;
}

function getJsonFilters() {
//	alert(filters.filters.length);
	filters.saveCurrentFilter(NaturalText.currentFilter);
	return JSON.stringify(filters);
}

Filters.prototype.asText = function(filter) {
	var s = "";
//	columns =
	for(var i = 0; i < filter.filterLines.length; i++) {
		s += filter.filterLines[i].clude + " <strong>" + this.getColumns()[filter.filterLines[i].column].filterInfo.name + "</strong> " + NaturalText[filter.filterLines[i].match].toLowerCase() + " <strong>'" + filter.filterLines[i].value + "'</strong>";
		if (i < filter.filterLines.length-1) {
			s += " " + filter.filterLines[i].connect + " ";
		}
	}
	s = s.charAt(0).toUpperCase() + s.substr(1);
	return s + ".";
}

Filters.prototype.asSql = function(filter) {
	var s = "";
	columns = this.getColumns();
//	prompt("", JSON.stringify(columns));
	for(var i = 0; i < filter.filterLines.length; i++) {
		filterInfo = columns[filter.filterLines[i].column].filterInfo;

/// ToDo Special selects:
//
//	Similar time of year:
//		Current week across all years:
//			select * from filenodes where strftime('%W', `modified`) = strftime('%W', 'now')
//		+/- ## days across all years

// select * from filenodes where not size < 50000
// select * from filenodes where not path LIKE '%marijn%'
		//s +=  + "</strong> " + NaturalText[filter.filterLines[i].match].toLowerCase() + " <strong>'" + filter.filterLines[i].value + "'</strong>";

		if (filterInfo.varType != 1) {
			if (filter.filterLines[i].clude == "exclude") s += " NOT ";
			s += "`" + filter.filterLines[i].column + "`";
		}
		switch(filterInfo.varType) {
			case undefined:
			break;
			case 0:
				var start = "%";
				var end = "%";
				switch(filter.filterLines[i].match) {
					case "begin": start = ''; break;
					case "ends": end = ''; break;
				}
				// ToDo: Escape filter.filterLines[i].value

				s += " LIKE '" + start + sqlite_escape_string(filter.filterLines[i].value) + end + "' ESCAPE '\\'";
				//s += " LIKE '" + start + sqlite_escape_string(filter.filterLines[i].value) + end + "'";
				//s += " LIKE '" + start + filter.filterLines[i].value + end + "'";
			break;
			case 1:
				s += " " + filter.filterLines[i].value + " ";
			break;
			default:
				var compare;
				switch(filter.filterLines[i].match) {
					case "less": compare = "<"; break;
					case "greater": compare = ">"; break;
					default: compare = "="; break;
				}
				s += " " + compare + " " + filter.filterLines[i].value;

			break;
		}

		//if (this.getColumns()[filter.filterLines[i].column].filterInfo.

		if (i < filter.filterLines.length-1) {
			s += " " + filter.filterLines[i].connect.toUpperCase() + " ";
		}
	}
	return s;
}

Filters.prototype.showFilterList = function() {
	$("#filters").empty();
	for(var i = 0; i < filters.filters.length; i++) {
		var currentFilter = "";
		if (filters.filters[i].name == NaturalText.currentFilter) currentFilter = " currentFilter";
		$("#filters")
			.append($("<div/>").addClass("filter" + currentFilter)
				.append($("<div/>").addClass("actions").attr("data-name", filters.filters[i].name).html('<a class="selectFilter button" href="#">Select</a> <a class="removeFilter button" title="Remove filter button" href="#">X</a>'))
				.append($("<div/>").addClass("name").html(filters.filters[i].name))
				.append($("<div/>").addClass("content").html(this.asText(filters.filters[i])))
				//.append($("<div/>").addClass("sql").html(this.asSql(filters.filters[i])))
			)
		;
		$(".button").button()
	}
	$(".selectFilter").click(function() {
		filters.edit($(this).parent().attr("data-name"));
		$("#filterName").val($(this).parent().attr("data-name"));
		settingChanged($("#filterName")[0]);
		if ($("#useFilter").prop("checked")) applyFilter();
	});

	$(".removeFilter").click(function() {
		filters.remove($(this).parent().attr("data-name"));
	});
}

Filters.prototype.getColumns = function() {
	if (this.columns == false) {
		if (typeof(window.external.jsGetFilterColumns) !== "undefined") {
			//prompt("", window.external.jsGetFilterColumns());
			this.columns = JSON.parse(window.external.jsGetFilterColumns());
		} else {
			this.columns = JSON.parse('			{"id":{"type":"INTEGER PRIMARY KEY AUTOINCREMENT","filterInfo":{"name":"","searchable":false,"varType":0},"index":true,"unique":true,"searchable":false},"path":{"type":"TEXT UNIQUE","filterInfo":{"name":"paths","searchable":true,"varType":0},"index":true,"unique":true,"searchable":false},"parentpath":{"type":"TEXT","filterInfo":{"name":"parent paths","searchable":true,"varType":0},"index":true,"unique":false,"searchable":false},"filename":{"type":"TEXT","filterInfo":{"name":"filenames","searchable":true,"varType":0},"index":true,"unique":false,"searchable":false},"created":{"type":"DATETIME","filterInfo":{"name":"created date","searchable":true,"varType":3},"index":true,"unique":false,"searchable":false},"modified":{"type":"DATETIME","filterInfo":{"name":"modified date","searchable":true,"varType":3},"index":true,"unique":false,"searchable":false},"size":{"type":"INTEGER","filterInfo":{"name":"size","searchable":true,"varType":2},"index":true,"unique":false,"searchable":false},"metainfoindexed":{"type":"INTEGER DEFAULT 0","filterInfo":{"name":"","searchable":false,"varType":0},"index":false,"unique":false,"searchable":false},"all":{"type":"TEXT","filterInfo":{"name":"metadata","searchable":true,"varType":0},"index":false,"unique":false,"searchable":false},"width":{"type":"TEXT","filterInfo":{"name":"width","searchable":true,"varType":2},"index":false,"unique":false,"searchable":false},"height":{"type":"TEXT","filterInfo":{"name":"height","searchable":true,"varType":2},"index":false,"unique":false,"searchable":false},"area":{"type":"DATETIME","filterInfo":{"name":"area","searchable":true,"varType":2},"index":false,"unique":false,"searchable":false},"raw":{"filterInfo":{"name":"raw SQL","searchable":true}}}');
		}
	}
	this.columns.raw = {
		"filterInfo": {
			"name": "raw SQL",
			"searchable": true,
			"varType": 1
		}
	}
	return this.columns;
};

Filters.prototype.add = function(filter) {
	this.filters.push(filter);
}

Filters.prototype.findKey = function(name) {
	for (var i = 0; i < this.filters.length; i++) {
		if (name == this.filters[i].name) return i;
	}
	return undefined;
}

Filters.prototype.saveCurrentFilter = function(key) {
	var filterLines = [];
	for(var i = 1; i <= this.editorCount; i++) {
		var line = new FilterLine();
		filterLines.push(line.getEditorValues(i));
	}
	var id = filters.findKey(key);
	if (id == undefined) {
		filters.add(new Filter(key, filterLines));
	} else {
		filters.filters[id] = new Filter(key, filterLines);
	}
	filters.showFilterList();
}

Filters.prototype.remove = function(name) {
	this.currentFilter = this.findKey(name);
	if (this.currentFilter == undefined) {
		alert("Filter '" + name + "' not found.");
		return undefined;
	} else {
		this.filters.splice(this.currentFilter, 1);
		filters.showFilterList();
	}
}

Filters.prototype.edit = function(name) {
	this.currentFilter = this.findKey(name);
	if (this.currentFilter == undefined) {
		alert("Filter '" + name + "' not found.");
		return undefined;
	}
	$("#filterLines").empty();
	this.editorCount = 0;
//	$("#filterNrLines").val(this.filters[key].filterLines.length);
	for(var i = 0; i < this.filters[this.currentFilter].filterLines.length; i++) {
		this.editorAddLine(this.filters[this.currentFilter].filterLines[i]);
	}
	this.currentFilter = this.findKey(NaturalText.currentFilter);
}

Filters.prototype.editorRemoveLine = function(removeId) {
//	$("#fl_" + removeId).remove();
	var filterLines = [];
	$("#fl_" + removeId).remove();
	for(var i = parseInt(removeId)+1; i <= this.editorCount; i++) {
		var line = new FilterLine();
		filterLines.push(line.getEditorValues(i));
		$("#fl_" + i).remove();
	}

	this.editorCount = removeId-1;
	for(var i = 0; i < filterLines.length; i++) {
		this.editorAddLine(filterLines[i]);
	}
}

Filters.prototype.editorAddLine = function(line) {
	this.editorCount++;

	var $columnSelect = $("<select>")
		.attr("id", "column_" + this.editorCount)
		.attr("data-id", this.editorCount)
		.addClass("columns text ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only");

	columns = filters.getColumns();

	for(var key in columns) {
		var value = columns[key];
		if (columns[key].filterInfo.searchable) {
			$columnSelect.append($("<option value='" + key + "' class='column vt" + columns[key].filterInfo.varType + "'>" + columns[key].filterInfo.name + "</option>"));
		}
	}

	var $newFilterLine = $("<div/>")
		.attr("id", "fl_" + this.editorCount)
		.addClass("filterLine")
		.append($("<div/>").addClass("line").attr("id", "l_" + this.editorCount)
			.append($("<select id='clude_" + this.editorCount + "' class='clude ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only'><option value='include'>Include</option><option value='exclude'>Exclude</option></select>"))
			.append($columnSelect)
			.append($("<select id='match_text_" + this.editorCount + "' class='ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only match isText'>"
									 + "<option value='contains'>" + NaturalText.contains + "</option>"
									 + "<option value='begin'>" + NaturalText.begin + "</option>"
									 + "<option value='ends'>" + NaturalText.ends + "</option>"
								+ "</select>")
			)
			.append($("<select id='match_numeral_" + this.editorCount + "' class='ui-button ui-widget ui-state-default ui-corner-all ui-button-text-only match isNumeral'>"
									 + "<option value='equals'>" + NaturalText.equals + "</option>"
									 + "<option value='less'>" + NaturalText.less + "</option>"
									 + "<option value='greater'>" + NaturalText.greater + "</option>"
								+ "</select>")
			)
			.append($("<input type='text' id='value_" + this.editorCount + "' value='' class='value'/>"))
			.append($("<a data-id='" + this.editorCount + "' class='remove_line' title='Delete dimension' href='#'>[X]</a>"))
		)
		.append($("<div class='connect'><label for='connect_and_" + this.editorCount + "'><input type='radio' id='connect_and_" + this.editorCount + "' name='connect_" + this.editorCount + "' value='and' checked/> and</label><label for='connect_or_" + this.editorCount + "'><input type='radio' id='connect_or_" + this.editorCount + "' name='connect_" + this.editorCount + "' value='or'/> or</label></div>")
	);

	//$newFilterLine.insertBefore($("#add_filter"));
	$("#filterLines").append($newFilterLine);

	if (typeof(line) != "undefined") {
		$("#clude_" + this.editorCount, $newFilterLine).val(line.clude);
		$("#column_" + this.editorCount, $newFilterLine).val(line.column);
		$("#match_text_" + this.editorCount, $newFilterLine).val(line.match);
		$("#match_numeral_" + this.editorCount, $newFilterLine).val(line.match);
		$("#value_" + this.editorCount, $newFilterLine).val(line.value);
		if (line.connect == "and") $("#connect_and_" + this.editorCount, $newFilterLine).prop('checked', true);
		else $("#connect_or_" + this.editorCount, $newFilterLine).prop('checked', true);
	}

	$(".remove_line", $newFilterLine).click(function() {
		filters.editorRemoveLine($(this).attr("data-id"));
	});

	$("#l_" + this.editorCount + " .columns").change(function() {
		$("#l_" + $(this).attr("data-id")).removeClass();
		var match = "Text";
		if (columns[this.value].filterInfo.varType >= 2) match = "Numeral";
		if (columns[this.value].filterInfo.varType == 1) match = "RawSQL";
		$("#l_" + $(this).attr("data-id")).addClass("line set" + match);
	});

	$("#l_" + this.editorCount + " .columns").change();
	$("#filterNrLines").val($("#tabFilters .filterLine").length-1);
	settingChanged($("#filterNrLines")[0]);
};

// Source: http://stackoverflow.com/questions/7744912/making-a-javascript-string-sql-friendly
function sqlite_escape_string(str) {
	return str.replace(/'/g, "''").replace(/[\0\x08\x09\x1a\n\r_\\\%]/g, function (char) {
		switch (char) {
			case "\0":
				return "\\0";
			case "\x08":
				return "\\b";
			case "\x09":
				return "\\t";
			case "\x1a":
				return "\\z";
			case "\n":
				return "\\n";
			case "\r":
				return "\\r";
			case "_":
			case "\\":
			case "%":
				return "\\"+char; // prepends a backslash to backslash, percent,
													// and double/single quotes
		}
	});
}

function beep() {
	if (typeof Audio !== "undefined") {
		var snd = new Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");
		snd.play();
	}
}

function hideWaiting() {
	$("#waiting").hide();
}

// Called from C# when config form is first shown.
function initFancyTreeFolder() {
	//alert("function initFancyTreeFolder()");
	$("#foldersFancyTree").fancytree({
		extensions: ["menu", "rps"/*"excludedSubfolders"/*, "filter"*/],
		checkbox: true,
		selectMode: 3,
		source: function() {
			if (typeof(window.external.getInitialFoldersJSON) !== "undefined") {
				return JSON.parse(window.external.getInitialFoldersJSON(0));
			} else {
				return $.getJSON("./js/fileSystemExample.json");
			}
		},
		lazyLoad: function(event, data) {
			this.fixSelection3 = true;
			if (typeof(window.external.getFolderJSON) !== "undefined") {
				var json = window.external.getFolderJSON(getFullPathFromFancytreeNode(data.node));
				data.result = JSON.parse(json);
			} else {
				data.result = $.getJSON("./js/fileSystemExample.json");
			}
		},
		loadChildren: function(event, ctx) {
			if (this.fixSelection3) {
				ctx.node.fixSelection3AfterClick();
			}
			updateExcludedFolders(ctx);
		},
		select: function(event, data) {
			// Get a list of all selected nodes, and convert to a key array:
			var selKeys = $.map(data.tree.getSelectedNodes(), function(node){
				return node.key;
			});
			$("#echoSelection3").text(selKeys.join(", "));

			// Get a list of all selected TOP nodes
			//var selRootNodes = ;
			// ... and convert to a key array:
			var selRootKeys = $.map(data.tree.getSelectedNodes(true), function(node){
				return node.key;
			});

			//$("#echoSelectionRootKeys3").text(selRootKeys.join(", "));
			//$("#echoSelectionRoots3").text(selRootNodes.join(", "));

			var folders = [];

			$.each(data.tree.getSelectedNodes(), function() {
				if (selRootKeys.indexOf(this.key) > -1) {
					var folder = getFullPathFromFancytreeNode(this);
					folders.push(folder);
					this.extraClasses = "root-node";
				} else {
					this.extraClasses = "not-root-node";
				}
				this.renderTitle();
			});
			$("#folders").text(folders.join("\n").replace(/\\\\/g, "\\"));
			settingChanged($("#folders")[0]);
			updateExcludedFolders();
		},
		dblclick: function(event, data) {
			data.node.toggleSelected();
		},
		keydown: function(event, data) {
			if( event.which === 32 ) {
				data.node.toggleSelected();
				return false;
			}
		},
/*	    filter: {
			mode: "dimm"
		},*/
		menu: {
			selector: "#myMenu",
			position: {my: "center"},
			create: function(event, data){
				//$.ui.fancytree.debug("Menu create ", data.$menu);
			},
			beforeOpen: function(event, data){
					data.$menu.context.innerHTML = data.$menu.context.innerHTML.replace(/#subfolder#/g, data.node.key);
					if (data.node.extraClasses != undefined && data.node.extraClasses.indexOf("excluded") > -1) {
						data.$menu.addClass("excluded");
					} else {
						data.$menu.removeClass("excluded");
					}
				//$.ui.fancytree.debug("Menu beforeOpen ", data.$menu, data.node);
			},
			open: function(event, data){
				//$.ui.fancytree.debug("Menu open ", data.$menu, data.node);
			},
			focus: function(event, data){
				//$.ui.fancytree.debug("Menu focus ", data.menuId, data.node);
			},
			select: function(event, data){
				switch(data.menuId) {
					case "#include":
						// remove item from exclude list
						excludedFolders = $("#excludedSubfolders").val().trim().split("\n");
						excludedFolders = excludedFolders.filter(function(i) { return i != data.node.key; });
						$("#excludedSubfolders").val(excludedFolders.join("\n"));
						removeExclusion(data.node.key);
					break;
					case "#exclude":
						excludedFolders = $("#excludedSubfolders").val().trim().split("\n");
						excludedFolders.push(data.node.key);
						$("#excludedSubfolders").val(excludedFolders.join("\n"));
					break;
				}
				settingChanged($("#excludedSubfolders")[0]);
				updateExcludedFolders();
				//alert("Menu select " + data.menuId + ", " + data.node);
			},
			close: function(event, data){
				//$.ui.fancytree.debug("Menu close ", data.$menu, data.node);
			}
		},

		// The following options are only required, if we have more than one tree on one page:
//				initId: "treeData",
		cookieId: "fancytree-Cb3",
		idPrefix: "fancytree-Cb3-"
	});
}

function initFancyTreeTransitions() {
	$("#transitionsFancyTree").fancytree({
		extensions: ["menu", "rps"/*"excludedSubfolders"/*, "filter"*/],
		checkbox: true,
		selectMode: 3,
		source: function() {
			if (typeof(window.external.jsGetSelectedEffects) !== "undefined") {
				return JSON.parse(window.external.jsGetSelectedEffects());
			} else {
				return $.getJSON("./js/effects.json");
			}
		},
		select: function(event, data) {
			if (typeof(window.external.jsSetSelectedEffects) !== "undefined") {
				return window.external.jsSetSelectedEffects(JSON.stringify(data.tree.toDict(true)));
			} else {
				alert(JSON.stringify(data.tree.toDict(true)));
			}
		},
		cookieId: "fancytree-Cb4",
		idPrefix: "fancytree-Cb4-"
	});
}

function getChildFromPathElements(pathElements, node, addNode) {
	if (addNode == undefined) addNode = false;
//		if (node.children == undefined || node.children == null)
//			return null;
	if (node.children != undefined) {
		if (pathElements[0].indexOf(":") > -1) pathElements[0] = pathElements[0] + "\\";
		for(var i = 0; i < node.children.length; i++) {
			if (pathElements[0] == node.children[i].key.toLowerCase()) {
				if (pathElements.length == 1) return node.children[i];
				pathElements.shift();
				return getChildFromPathElements(pathElements, node.children[i], addNode);
			}
		}
	}
	if (addNode) {
		node = node.addChildren([{
			title: pathElements[0],
			key: pathElements[0],
			folder: true
		},{
			title: "dummy",
			extraClasses: "hidden"
		}]);
		if (pathElements.length >= 1) pathElements.shift();
		if (pathElements.length == 0) return node;
		else return getChildFromPathElements(pathElements, node, addNode);
	}
}

function getFancytreeNodeFromFullPath(path, addNode) {
	if (addNode == undefined) addNode = false;
	path = path.toLowerCase();
	if (path.substring(0, 2) == "\\\\") {
		path = path.replace("\\\\", "network\\");
	} else {
		path = "computer\\" + path;
	}
	pathElements = path.replace(/\\\\/g, "\\").trim().split("\\");
	while(pathElements[pathElements.length-1] == "") pathElements.pop();
	return getChildFromPathElements(pathElements, $("#foldersFancyTree").fancytree("getTree").rootNode, addNode);
}

function getFullPathFromFancytreeNode(node) {
		var folders = node.getParentList(false, true);
		var folder = "";
		$.each(folders, function() {
			if (this.getLevel() > 0) {
				folder += this.key;
				if (this.key[this.key.length-1] != '\\') folder += "\\";
			}
		});
		//alert(folders);
		var type = folder.split("\\", 1)[0].toLowerCase();
		switch(type) {
			case "computer":
				folder = folder.replace("computer\\", "");
			break;
			case "network":
				folder = folder.replace("network", "\\");
			break;
		}
		return folder;
}

function removeExclusion(folder) {
	$.each($("#foldersFancyTree").fancytree("getTree").rootNode.findByKey(folder), function() {
		if (this.extraClasses != null) this.extraClasses = this.extraClasses.replace(/excluded/g, "");
		this.renderTitle();
	});
}

function updateExcludedFolders() {
	excludedFolders = $("#excludedSubfolders").val().trim().split("\n");

	// Clear old selection
	$.each($("#foldersFancyTree").fancytree("getTree").rootNode.findAll(""), function() {
		if (this.extraClasses != undefined && this.extraClasses != null) this.extraClasses = this.extraClasses.replace(/excluded/g, "");
	});

	$.each(excludedFolders, function() {
		$.each($("#foldersFancyTree").fancytree("getTree").rootNode.findByKey(this), function() {
			if (this.extraClasses == undefined) this.extraClasses = "excluded";
			else {
				if (this.extraClasses.indexOf("excluded") == -1) this.extraClasses += " excluded";
			}
			this.renderTitle();
		});
	});
}

function applyFilter() {
	filters.saveCurrentFilter(NaturalText.currentFilter);
	var key = filters.findKey(NaturalText.currentFilter);
	$("#filter").val(filters.asSql(filters.filters[key]));
	settingChanged($("#filter")[0]);
	if (typeof(window.external.jsApplyFilter) !== "undefined") {
		return window.external.jsApplyFilter($("#filter").val());
	} else {
		return alert("Can't apply '" + $("#filter").val() + "' filter in preview browser mode");
	}
}

function saveFilter() {
	if (typeof(window.external.jsSaveFilter) !== "undefined") {
		return window.external.jsSaveFilter($("#filter").val());
	} else {
		return alert("Can't save filter in preview browser mode");
	}
}

function clearFilter() {
//	$("#useFilter").attr("checked", false);
	if (typeof(window.external.jsClearFilter) !== "undefined") {
		return window.external.jsClearFilter("dummy");
	} else {
		return alert("Can't clear filter in preview browser mode");
	}
}

function checkRawConverter() {
	var found = false;
	if (typeof(window.external.jsRawConverterAvailable) !== "undefined") {
		if (window.external.jsRawConverterAvailable($("#rawConverter").val()) == "true") {
			found = true;
		}
	}
	if (found) {
		$("#rawUseConverter").attr("disabled", false);
		$(".noRaw").hide();
	} else {
		$("#rawUseConverter").attr("disabled", true);
		$("#rawUseConverter").attr("checked", false);
		$(".noRaw").show();
	}
}

/**
 * Called from RPS
 **/
function persistantConfigLoaded() {
	checkRawConverter();
	$("#backgroundColour").spectrum("set", $("#backgroundColour").val());
	$("#wallpaperBackgroundColour").spectrum("set", $("#wallpaperBackgroundColour").val());
	$(".subOptions").each(function() {
		$(this).change();
	});
}

function settingChanged(object) {
	id = object.id;
	value = null;
	//beep();
	switch($(object).attr("type")) {
		case "checkbox":
			// Set to numeric value (international version translate True and False)
			if (object.checked) value = 1; else value = 0;
		break;
		case "radio":
			id = object.name;
			value = object.value;
		break;
		default:
			value = object.value;
		break;
	}
	if (typeof(window.external.jsInputChanged) !== "undefined") {
		window.external.jsInputChanged(id, value);
	}
}

jQuery.fn.highlightSlow = function () {
	$(this).each(function () {
		$(this).animate({ backgroundColor: "#ffff99" }, 500 )
					 .animate({ backgroundColor: "#ffff99" }, 2000 )
					 .animate({ backgroundColor: "white" }, 1500 )
	});
}

$(function(){
	if ($('html').is('.lowIE')) {
		alert('Please update to Internet Explorer 8 or newer');
			oldIE = true;
	}

	// Rerun on refresh
	persistantConfigLoaded();

	/** Begin Tabs **/
	var tabs = $('#tabs > li');
	var content = $('#tabs-content > li');

	tabs.bind('click',function(){
		content.hide(); //hide all contents
		tabs.removeClass('current'); //remove 'current' classes
		$("#" + $(this).attr("data-tab")).show(); //show tab content that matches tab title index
		$(this).addClass('current'); //add current class on clicked tab title
	});

	if (window.location.hash.length > 0) {
		var tab = window.location.hash.replace("#tab", "").replace("#", "");
		tab = "tab" + tab.charAt(0).toUpperCase() + tab.substr(1).toLowerCase();
		$("#tabs").find("[data-tab='" + tab + "']").click();
	} else {
		tabs.first().click();
	}

	// Select a tab and highlights a field
	$(".goToTab").click(function() {
		var highlight = null;
		var tab = this.href.slice(this.href.lastIndexOf('#')+1);
		highlight = $(this).data("highlight");//hash[1];//.split(",");

		tab = "tab" + tab.charAt(0).toUpperCase() + tab.substr(1).toLowerCase();
		$("#tabs").find("[data-tab='" + tab + "']").click();
		if (highlight != null) {
			$(highlight).highlightSlow();
		}
	});

	//$("#tabs").find("[data-tab='tabFilters']").click();

	/** End Tabs **/

	/** Begin Advanced **/
	var showText = "Show advanced settings...";
	var hideText = "Hide advanced settings...";

	$(".advanced").after("<a href='#' class='toggleAdvanced'>" + showText + "</a>");

	$(".toggleAdvanced").click(function() {
		if ($(".advanced").is(":visible")) $(".toggleAdvanced").text(showText);
		else $(".toggleAdvanced").text(hideText);
		$(".advanced").toggle('slow');
		return false;
	});
	/** End Advanced **/


//alert($("#backgroundColour").val());
	$("#backgroundColour, #wallpaperBackgroundColour").spectrum({
			showInput: true,
			className: "full-spectrum",
			showInitial: true,
			showPalette: true,
			showSelectionPalette: true,
			clickoutFiresChange: true,
			maxPaletteSize: 10,
			preferredFormat: "hex",
			localStorageKey: "spectrum.demo",
			move: function (color) {

			},
			show: function () {

			},
			beforeShow: function () {

			},
			hide: function () {

			},
			change: function() {

			},
			palette: [
					["rgb(0, 0, 0)", "rgb(67, 67, 67)", "rgb(102, 102, 102)",
					"rgb(204, 204, 204)", "rgb(217, 217, 217)","rgb(255, 255, 255)"],
					["rgb(152, 0, 0)", "rgb(255, 0, 0)", "rgb(255, 153, 0)", "rgb(255, 255, 0)", "rgb(0, 255, 0)",
					"rgb(0, 255, 255)", "rgb(74, 134, 232)", "rgb(0, 0, 255)", "rgb(153, 0, 255)", "rgb(255, 0, 255)"],
					["rgb(230, 184, 175)", "rgb(244, 204, 204)", "rgb(252, 229, 205)", "rgb(255, 242, 204)", "rgb(217, 234, 211)",
					"rgb(208, 224, 227)", "rgb(201, 218, 248)", "rgb(207, 226, 243)", "rgb(217, 210, 233)", "rgb(234, 209, 220)",
					"rgb(221, 126, 107)", "rgb(234, 153, 153)", "rgb(249, 203, 156)", "rgb(255, 229, 153)", "rgb(182, 215, 168)",
					"rgb(162, 196, 201)", "rgb(164, 194, 244)", "rgb(159, 197, 232)", "rgb(180, 167, 214)", "rgb(213, 166, 189)",
					"rgb(204, 65, 37)", "rgb(224, 102, 102)", "rgb(246, 178, 107)", "rgb(255, 217, 102)", "rgb(147, 196, 125)",
					"rgb(118, 165, 175)", "rgb(109, 158, 235)", "rgb(111, 168, 220)", "rgb(142, 124, 195)", "rgb(194, 123, 160)",
					"rgb(166, 28, 0)", "rgb(204, 0, 0)", "rgb(230, 145, 56)", "rgb(241, 194, 50)", "rgb(106, 168, 79)",
					"rgb(69, 129, 142)", "rgb(60, 120, 216)", "rgb(61, 133, 198)", "rgb(103, 78, 167)", "rgb(166, 77, 121)",
					"rgb(91, 15, 0)", "rgb(102, 0, 0)", "rgb(120, 63, 4)", "rgb(127, 96, 0)", "rgb(39, 78, 19)",
					"rgb(12, 52, 61)", "rgb(28, 69, 135)", "rgb(7, 55, 99)", "rgb(32, 18, 77)", "rgb(76, 17, 48)"]
			]
	});

	$( ".linkButton" ).button();

	$( ".button" )
		.button()
		.click(function( event ) {
			event.preventDefault();
	});

	/**
	 * Disable sub options for radio groups
	 **/
	$(".subOptions").change(function() {
		$("input[type=radio]", this).each(function() {
			var disabled = false;
			if ($("input:radio[name=" + $(this).attr("name") + "]:checked").attr("id") != undefined) disabled = (this.id != $("input:radio[name=" + $(this).attr("name") + "]:checked").attr("id").toString());
			$("." + this.id + " input").each(function() { $(this).attr("disabled", disabled); });
			$("." + this.id).each(function() {
				$(this).toggleClass("disabled", disabled);
		});
		});
	});

	$(".browseFile .button").click(function() {
		var id = this.id.replace("Browse", "");
		if (typeof(window.external.jsFileBrowserDialog) === "undefined") {
			alert("Can't display folder browse dialog in browser preview mode");
		} else {
			$("#" + id).val(window.external.jsFileBrowserDialog($("#" + id).val(), $("#" + id).attr("data-filter")));
		}
	});

	$("#debugLogLocation").click(function() {
		try {
			window.external.jsOpenProgramAppDataFolder();
		} catch(e) {
		}
	});

	$("#add_filter_dimension").click(function() {
		filters.editorAddLine();
	});

	$("#saveFilter").click(function() {
		filters.saveCurrentFilter($("#filterName").val());
		settingChanged($("#filterName")[0]);
	});

	$("#save").click(function() {
		try {
			window.external.savePersistantConfig();
			beep();
		} catch(e) {
			alert("Can't automatically save settings in browser preview mode");
		}
	});

	$("#detectUFRaw").click(function() {
		if (typeof(window.external.jsGetUFRawLocation) === "undefined") {
			alert("Can't automatically get UFRaw location in browser preview mode");
		} else {
			path = window.external.jsGetUFRawLocation();
			if (path.length > 0) {
				$("#rawConverter").val(path);
			} else {
				alert("UFRaw location not automatically detected. Set location of 'ufraw-batch.exe' manually?");
			}
			checkRawConverter();
		}
	});

	$("#rawConverter").on('change keyup' ,function(){
		checkRawConverter();
	});

	$(".hideMessage").click(function() {
		$("#donation").hide();
	});

	$("#useFilter").change(function() {
		if ($("#useFilter").checked == true) {
			clearFilter();
		} else {
			applyFilter();
		}
	});

	$("#clearWallpaper").click(function() {
		if (typeof(window.external.resetWallpaper) !== "undefined") {
			window.external.resetWallpaper();
		}
		alert("Wallpaper reset");
	});

	$("#applyFilter").click(function() {
		$("#useFilter").prop("checked", true);
		applyFilter();
	});

	$("#clearFilter").click(function() {
		$("#useFilter").prop("checked", false);
		clearFilter();
	});

	$(".expandSelected").click(function() {
		if (!$("#tree").is(":visible")) {
			$(".toggleTreeTextareas").click();
		}
		$.each($("#foldersFancyTree").fancytree("getTree").getSelectedNodes(true), function(node){
			this.makeVisible(true);
		});
	});

	$(".toggleTreeTextareas").click(function() {
		$("#textareas textarea").height($("#foldersFancyTree").height());
		if ($("#tree").is(":visible")) {
			var folders = "";
			$.each($("#foldersFancyTree").fancytree("getTree").getSelectedNodes(true), function(node){
				folders += getFullPathFromFancytreeNode(this) + "\n";
			});
			$('#folders').val(folders.trim());
		} else {
			// Call $('#folders') before $.each as IE looses value after call!?!
			folders = $('#folders').val().trim().split("\n");

			$.each($("#foldersFancyTree").fancytree("getTree").getSelectedNodes(true), function(node){
				this.setSelected(false);
			});

			for(var i = 0; i < folders.length; i++) {
				node = getFancytreeNodeFromFullPath(folders[i], true);
				if (node != null) {
					node.setSelected(true);
				}
			}
			removeExclusion("");
			updateExcludedFolders();
		}

		$("#tree").toggle();
		$("#textareas").toggle();
		var text = $(".toggleTreeTextareas span").text();
		$(".toggleTreeTextareas span").text($(".toggleTreeTextareas").attr("data-toggle"));
		$(".toggleTreeTextareas").attr("data-toggle", text);
		//alert( "Handler for .click() called." );
	});

	if (typeof(window.external.getInitialFoldersJSON) === "undefined") {
		// Manually initiale on in browser preview
		initFancyTreeFolder();
		initFancyTreeTransitions();
		updateExcludedFolders();
	}

	$("#backgroundColour").change(function() {
		if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
			window.external.InvokeScriptOnMonitor(-1, "setBackgroundColour", $("#backgroundColour").val());
		}
	});

	$(".pathParts input").change(function() {
		if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
			var params = [ "." + $(this).data("value"), this.checked ];
			window.external.InvokeScriptOnMonitor(-1, "toggle", params.join(";"));
		}
	});

	$(".external, .external a").click(function(event) {
		if (typeof(window.external.jsOpenExternalLink) !== "undefined") {
			window.external.jsOpenExternalLink($(this).attr("href"));
			event.preventDefault();
		}
	});

/*
	$.each(excludedFolders, function() {
		$.each($("#foldersFancyTree").fancytree("getTree").rootNode.findAll(this), function() {
			this.extraClasses = "excluded";
			this.renderTitle();
		});
	});
*/
	/*
	$("#foldersFancyTree").fancytree("getTree").filterNodes(function(node) {
		return new RegExp("800x600", "i").test(node.title);
	});*/
/*
	$("#foldersFancyTree").fancytree("getRootNode").visit(function(node){
		node.setExpanded(true);
	});
*/
	$("#waiting").click(function() {
		if (typeof(window.external.InvokeScriptOnMonitor) == "undefined") {
			$("#waiting").hide();
		}
	});


});

function searchSettings(event) {
	var sets = document.querySelectorAll(".set");
	if (sets != undefined) {
		for(var i = 0; i < sets.length; i++) {
			if (sets[i].textContent.toLowerCase().contains(this.value.toLowerCase())) {
				sets[i].style.display = 'block';
			} else {
				sets[i].style.display = 'none';
			}
		}
		//$(sets).removeHighlight();
		//$(sets).highlight(this.value.toLowerCase());
	}
}

function initMonitors(count) {
	if (count > 1) {
		var m1 = $("#M1")[0];
		var h = m1.outerHTML ;
		for(i = count; i > 1; i--) {
			$(h.replace(/M1/g, "M" + i).replace("Monitor 1", "Monitor " + i)).insertAfter("#M1");
		}
	}
	$(".multimonitor input, .multimonitor textarea").bind('change keyup input', function() {
		if (typeof(window.external.InvokeScriptOnMonitor)=== "undefined") {
			alert("Not support on browser preview");
			return;
		}

		var monitor = this.id.match(/(.*)M([0-9]+)$/);
		var action = monitor[1];
		var monitor = monitor[2] - 1;

		switch(action) {
			case "showQuickMetadata":
				var params = [ "#quickMetadata", this.checked ];
				if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
					window.external.InvokeScriptOnMonitor(monitor, "toggle", params.join(";"));
				}
			break;
			case "showFilename":
				var params = [ "#filename", this.checked ];
				if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
					window.external.InvokeScriptOnMonitor(monitor, "toggle", params.join(";"));
				}
			break;
			case "showIndexProgress":
				var params = [ "#indexprogress", this.checked ];
				if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
					window.external.InvokeScriptOnMonitor(monitor, "toggle", params.join(";"));
				}
			break;

			case "noClock":
			case "currentClock":
			case "elapsedClock":
				var params = [ action.replace("Clock", "") ];
				if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
					window.external.InvokeScriptOnMonitor(monitor, "setClockType", params.join(";"));
				}
			break;
			case "clockFormat":
				if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
					window.external.InvokeScriptOnMonitor(monitor, "setClockFormat", $(this).val());
				}
			break;
/*
	$(".clockFormat").bind('change keyup input', function() {
		if (typeof(window.external.InvokeScriptOnMonitor) !== "undefined") {
			window.external.InvokeScriptOnMonitor(-1, "setClockFormat", $(this).val());
		}
	});*/

		}
	});

	// Assign change / keyups after monitors are initialised.
	$("input,textarea,select").change(function() {
		settingChanged(this);
	});

	// ToDo: Test whether this slows down text entry?
	$("input[type=text],textarea").keyup(function() {
		settingChanged(this);
	});

	$( "input[type=number]" ).spinner({
			min: 1,
			spin: function( event, ui ) { settingChanged(this); },
			change: function( event, ui ) { settingChanged(this); },
  });

	$( "#rememberLast" ).spinner({
			min: 0
	});
}

window.onload = function() {
	document.getElementById("search").onkeyup = searchSettings;
	initFilters();

	//document.getElementById("search").attachEvent("onchange", searchSettings);
/*
	$('#timeout,#timeoutMax').stepper({
			wheel_step:1,       // Wheel increment is 1
			limit: [1,]         // No negative values
	});*/
}

