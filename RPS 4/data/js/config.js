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
			//$("body, html").css('cursor', 'wait');
			//alert(data.node);
			var json = window.external.getFolderJSON(getFullPathFromFancytreeNode(data.node));
			data.result = JSON.parse(json);

			//$("body, html").css('cursor', 'auto');
		},
		loadChildren: function(event, ctx) {
			updateExcludedFolders(ctx);
			//ctx.node.fixSelection3AfterClick();
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
	if (typeof(window.external.applyFilter) !== "undefined") {
		return window.external.applyFilter($("#filter").val());
	} else {
		return alert("Can't apply '" + $("#filter").val() + "' filter in preview browser mode");
	}
}

function clearFilter() {
//	$("#useFilter").attr("checked", false);
	if (typeof(window.external.clearFilter) !== "undefined") {
		return window.external.clearFilter("dummy");
	} else {
		return alert("Can't clear filter in preview browser mode");
	}
}

$(function(){
	$( "#timeout,#timeoutMax" ).spinner({ min: 1 });

	$( ".linkButton" ).button();

	$( ".button" )
		.button()
		.click(function( event ) {
			event.preventDefault();
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

	$("#backgroundColour").change(function() {
		window.external.InvokeScriptOnMonitor(-1, "setBackgroundColour", $("#backgroundColour").val());
	});

	$("#clearWallpaper").click(function() {
		window.external.resetWallpaper();
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
		updateExcludedFolders();
	}

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
});

function searchSettings(event) {
	var sets = document.querySelectorAll(".set");
	if (sets != undefined) {
		for(var i = 0; i < sets.length; i++) {
			if (sets[i].innerText.toLowerCase().contains(this.value.toLowerCase())) {
				sets[i].style.display = 'block';
			} else {
				sets[i].style.display = 'none';
			}
		}
		$(sets).removeHighlight();
		$(sets).highlight(this.value.toLowerCase());
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
	$(".multimonitor input").change(function() {
		if (typeof(window.external.InvokeScriptOnMonitor)=== "undefined") {
			alert("Not support on browser preview");
			return;
		}

		var monitor = this.id.match(/(.*)M([0-9]+)$/);
		var action = monitor[1];
		var monitor = monitor[2];
		switch(action) {
			case "showQuickMetadata":
				var params = [ "#quickMetadata", this.checked ];
				window.external.InvokeScriptOnMonitor(monitor, "toggle", params.join(";"));
			break;
			case "showFilename":
				var params = [ "#filename", this.checked ];
				window.external.InvokeScriptOnMonitor(monitor, "toggle", params.join(";"));
			break;
		}

//		InvokeScriptOnMonitor(
		//alert(this.id);
	});
}

window.onload = function() {

	document.getElementById("search").onkeyup = searchSettings;
	//document.getElementById("search").attachEvent("onchange", searchSettings);
/*
	$('#timeout,#timeoutMax').stepper({
			wheel_step:1,       // Wheel increment is 1
			limit: [1,]         // No negative values
	});*/
}

