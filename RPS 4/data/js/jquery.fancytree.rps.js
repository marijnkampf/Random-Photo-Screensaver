/*!
 * jquery.fancytree.randomphotoscreensaver.js
 *
 * Add a child counter bubble to tree nodes.
 * (Extension module for jquery.fancytree.js: https://github.com//)
 *
 * Copyright (c) 2014, Marijn Kampf (http://abscreensavers.com)
 *
 * Released under the MIT license
 * https://github.com/mar10/fancytree/wiki/LicenseInfo
 *
 * @version @VERSION
 * @date @DATE
 */

 ;(function($, undefined) {

 "use strict";

	// TODO: expand(flag)
	/**Find all nodes that contain `match` in the title.
	 *
	 * @param {string} match string to search for
	 * @returns {FancytreeNode[]} array of nodes (may be empty)
	 * @see FancytreeNode#findAll
	 */
$.ui.fancytree._FancytreeNodeClass.prototype.findByKey = function(match){
		match = $.isFunction(match) ? match : this._makeNodeKeyMatcher(match);
		var res = [];
		this.visit(function(n){
			if(match(n)){
				res.push(n);
			}
		});
		return res;
};

$.ui.fancytree._FancytreeNodeClass.prototype._makeNodeKeyMatcher = function(s){
	s = s.toLowerCase();
	return function(node){
		return node.key.toLowerCase() == s;
	};
/*	s = s.toLowerCase();
	return this.key.toLowerCase() == s;*/
};


/* 'rps' extension */
$.ui.fancytree.registerExtension({
		name: "rps",
		version: "1.0.0",

	_makeNodeKeyMatcher: function(s){
		s = s.toLowerCase();
		return function(node){
			return node.title.toLowerCase().indexOf(s) >= 0;
		};
	},


	treeInit: function(ctx){
		var tree = this;

		this._super(ctx);
		//this.$container.addClass("fancytree-ext-childcounter");
	},

/*
	nodeRenderTitle: function(ctx, title) {
		var node = ctx.node,
			extOpts = ctx.options.childcounter,
			count = (node.data.childCounter == null) ? node.countChildren(extOpts.deep) : +node.data.childCounter;
		this._super(ctx, title);
		if( (count || ! extOpts.hideZeros) && (!node.isExpanded() || !extOpts.hideExpanded) ){
			$("span.fancytree-icon", node.span).append($("<span class='fancytree-childcounter'/>").text(count));
		}
	},
		*/
	treeDestroy: function(ctx){
		this._super(ctx);
	}
});

}(jQuery));

