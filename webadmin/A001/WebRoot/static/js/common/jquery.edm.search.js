//下拉搜索
;(function($){
	$.fn.extend({
		search:function(options){
			var defaults = {
				textId:'#' + this.attr('id'), /*默认打开源为本身*/
				valueId:'',                  /*放值元素的id*/
				boxId:'',
				json:'',
				createFn:$.fn.search.createSearch,  /*提供外用的搜索函数*/
				keyFn:$.fn.search.keySearch,
				itemFn:'',   //回调函数
				subArr:[],
				isHoriz:false,
				showSelectFn:'',
				cookieArr:'',
				sortFn:'',  //生成列表后的过滤函数,默认为历史记录操作
				outCate:'',  //排除类别
				deleteFn:''
			};
			
			var hasOrigPlace = 'placeholder' in document.createElement('input')?true:false;
			
			var p = $.extend({},defaults,options);
			
			initInput(p.textId,p.valueId,p.json,p.subArr);
			
			return this.each(function(){
				var $this = $(this);
				var $textId = $(p.textId);
				var $boxId = $(p.boxId);
				//绑定单个删除事件
				if(p.deleteFn == ''){
					$textId.on("keydown",function(e){
						return $.fn.search.deleteOne.call(this,e);
					});
				}else if(typeof p.deleteFn == 'function'){
					$textId.on("keydown",function(e){
						return p.deleteFn.call(this,e);
					});
				}
				
				//促成动态改变数据源
				$textId.data('json',p.json);
				//促成对外改变数据源
				$textId.data('subArr',p.subArr);
				//促成对外改变下拉框位置
				$textId.data("boxId",$boxId);
	
				$this.bind('click',function(){
					var json = $textId.data('json');
					$(".search_result",$boxId).html("");
					
					//点击设置搜索input的值为原来的值，针对ie7
					if(!hasOrigPlace){
						$(".search_input",$boxId).val($(".search_input",$boxId).attr("placeholder")||"");
					}else{
						$(".search_input",$boxId).val("");
					}
					
					//创建搜索
					p.createFn(json,p.itemFn,p.boxId,p.outCate);
					//有节点处理，不过这里影响性能
					if(p.sortFn){p.sortFn.call($boxId,$textId);};
					if(p.cookieArr){$.fn.search.cookieDisplay.call($boxId,$textId,p.cookieArr)};
					$.fn.search.posfix($textId,$boxId);	
					$boxId.setMaxIndex().show();
					
					//横向排列
					if(p.isHoriz){
						$boxId.addClass("searchHoriz");
						p.showSelectFn(p.textId);
					}else{
						//定义搜索
						$(".search_input",$boxId).unbind('keyup');
						$(".search_input",$boxId).bind('keyup',function(){
							var keyword = $(this).val().replace(/\s/g,"");
							if(keyword == "") {
								p.createFn(json,p.itemFn,p.boxId,p.outCate);
								if(p.sortFn){p.sortFn.call($boxId,$textId);};
								if(p.cookieArr){$.fn.search.cookieDisplay.call($boxId,$textId,p.cookieArr)};
							}else{
								p.keyFn(json,p.itemFn,p.boxId,keyword,p.outCate);
								if(p.sortFn){p.sortFn.call($boxId,$textId);};
							}	
						});
					}				
					
					$(document).bind('mousedown.search', function(e){  		
						if(!($(e.target).closest($boxId).length || 
							$(e.target).get(0) === $this.get(0) ||
							$(e.target).get(0) === $textId.get(0)
						)){
							$boxId.hide();	
							$(document).unbind('mousedown.search');	
						}			
					});				
					return false;
				})
			});		
		}		
	});	
	
	//提供对外的定位方法
	$.fn.search.posfix = function($textId,$boxId){
		$('.search_result',$boxId).css({'height':"auto"});
		var top = $textId.offset().top,
		 	left = $textId.offset().left,
		 	srcWidth = $textId.outerWidth(),
		 	srcHeight = $textId.outerHeight(),
			boxHeight = $boxId.outerHeight(),
			winHeight = $(window).height(),
			otherHeight = 85;
		
	    if(winHeight - top - srcHeight > boxHeight){
			$boxId.css({
				'left':left + 'px',
				'bottom':'auto',
				'top':top + srcHeight + 'px'
			});
		}else if((winHeight - top - srcHeight - otherHeight) > 70){
			$('.search_result',$boxId).height(function(){
				return winHeight - top - srcHeight - otherHeight;
			});
			$boxId.css({
				'left':left + 'px',
				'bottom':'auto',
				'top':top + srcHeight + 'px'
			}); 
		}else if(top > boxHeight){
			$boxId.css({
				'left':left + 'px',
				'top':'auto',
				'bottom':winHeight - top + 'px'
			});    
		}else{
			$boxId.css({
				'left':left + 'px',
				'top':0,
				'bottom':'auto'
			}); 
		}
	}
	
	$.fn.search.deleteOne = function(e){
		if(e.keyCode==8){
			$(this).val('');
			$(this).next().val('');
		}
		e.preventDefault();
	}
	
	//历史记录
	$.fn.search.cookieDisplay = function($textId,cookieArr){
		var name = $textId.next().attr('name');
		this.find('.search_result ul a').each(function(i){
			if($(this).attr('data-id') == cookieArr[name]){
				$(this).addClass('cookie').parents('li').prependTo('.ul_result');
			};
		});
	}
	
	//公共方法二维下拉搜索
	$.fn.search.createSearch=function(json,fn,boxId,outCate){
		var $boxId=$(boxId);
		var $resultId=$boxId.find(".search_result");
		var html="";
		if(typeof json.length=='number'){
			var obj=json;	
			html+="<ul class='ul_result'>";
			for(i=0;i<obj.length;i++){
				html+="<li>";
				html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
				for(var key in obj[i]){
					if(key!="title"){
						html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
					}	
				}
				html+=">" + obj[i].title + "</a>";
				html+="</li>";
			}
			html+="</ul>";
		}else{
			for(var prop in json){
				//排除类别
				if(prop == outCate){
					continue;
				}
				var obj=json[prop];
				if(obj.length > 0){
					html+="<h4 class=\"search_sort_title\" data-cate=\""+ prop +"\">";
					html+="<span class=\"icon_expand\">" + prop + "</span></h4>";	
				}
				
				html+="<ul data-cate=\"" + prop + "\">";
				for(i=0;i<obj.length;i++){
					html+="<li>";
					html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
					for(var key in obj[i]){
						if(key!="title"){
							html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
						}	
					}
					html+=">" + obj[i].title + "</a>";
					html+="</li>";
				}
				html+="</ul>";
			}
		}
		
		//先清空内容
		$resultId.html("").append(html).scrollTop(0);	
	}
	
	$.fn.search.keySearch=function(json,fn,boxId,val_txt,outCate){
		var $boxId=$(boxId);
		var $resultId=$boxId.find(".search_result");
		var html="";
		var counter=0;
		html+="<ul class='ul_result'>";
		if(typeof json.length=='number'){
			var obj=json;	
			for(i=0;i<obj.length;i++){
				var title=obj[i].title;
				var index1 = title.indexOf(val_txt);
				//搜索
				if(index1!=-1){ 
					title = title.replace(val_txt,'<b class="red_cate">'+val_txt+'</b>');
					html+="<li>";
					html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
					for(var key in obj[i]){
						if(key!="title"){
							html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
						}	
					}
					html+=">" + title + "</a>";
					html+="</li>";
					counter++;
				}
			}	
		}else{
			for(var prop in json){
				//排除类别
				if(prop == outCate){
					continue;
				}
				var obj=json[prop];
				for(i=0;i<obj.length;i++){
					var title=obj[i].title;
					var index1 = title.indexOf(val_txt);
					//搜索
					if(index1!=-1){ 
						title = title.replace(val_txt,'<b class="red_cate">'+val_txt+'</b>');
						html+="<li>";
						html+="<a href=\"javascript:void(0);\" onclick=\""+ fn +";\" title=\""+ obj[i].title +"\"";
						for(var key in obj[i]){
							if(key!="title"){
								html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
							}	
						}
						html+=">" + title + "</a>";
						html+="</li>";
						counter++;
					}
				}	
			}
		}
		html+="</ul>";
		//先清空内容
		$resultId.html("");
		if(counter){
			$resultId.append("<p class='flg' style='padding-left:10px'>搜索到以下结果：</p>");
			$resultId.append(html);
		}else{
			$resultId.append("<p class='flg' style='padding-left:10px'>没有匹配的结果！</p>");
		}
		$resultId.scrollTop(0);
	}

	function initInput(ele,valueEle,json,subArr){
			var id = $(valueEle).val();
			var $ele = $(ele);
			var flag = false;
			if(id == '' || typeof id == 'undefined'){
				return;
			}
			var textArr=[];
			
			if(typeof json.length =='number'){
				for(i=0;i<json.length;i++){
					var item = json[i];
					if( item.id==id || jQuery.inArray(item.id.toString(),id.split(','))!=-1 ){
						textArr.push(item.title);
						
						if(textArr.length==1){
							$ele.val(textArr[0]);
						}else if(textArr.length>1){
							$ele.val(textArr.join(', '));
						}
						
						if(typeof subArr == 'object'){
							if(item.info){
								var newarr=new Array(item.id, item.title, item.info);
							}else if(item.type){
								var newarr=new Array(item.id, item.title, item.type);
							}else{
								var newarr=new Array(item.id, item.title);
							}
							
							subArr.push(newarr);
						}	
						
						flag = true;
					}
				}
			}else{
				for(key in json){
					var obj = json[key];
					for(i=0;i<obj.length;i++){
						var item = obj[i];
						if( item.id==id || jQuery.inArray(item.id.toString(),id.split(','))!=-1 ){
							textArr.push(item.title);
														
							if(textArr.length==1){
								$ele.val(textArr[0]);
							}else if(textArr.length>1){
								$ele.val(textArr.join(', '));
							}
							
							if(typeof subArr == 'object'){
								if(item.info){
									var newarr=new Array(item.id, item.title, item.info);
								}else if(item.type){
									var newarr=new Array(item.id, item.title, item.type);
								}else{
									var newarr=new Array(item.id, item.title);
								}
								subArr.push(newarr);
							}
							
							flag = true;	
						}
					}	
				}
			}
			
			//标记id是否才在于数组，如果不存在把原来的设为空
			if(!flag){
				$(valueEle).val('');
			}
		}

})(jQuery);


//信息提示插件
;(function($){
	$.fn.extend({
		gtips:function(options){
			//默认生成的tips
			var divStr = '';
			 	divStr += '<div id="G_tips" class="global_tips">';
				divStr += '	<div class="content">';
				divStr += '	</div>';
				divStr += '	<div class="triangle-border top-triangle-border">';
				divStr += ' 	<div class="triangle top-triangle"></div>';
				divStr += ' </div>';							
				divStr += '</div>';
				
			var divStr2 = '';
			 	divStr2 += '<div id="G_info" class="slide_info">';
				divStr2 += '	<div class="content">';
				divStr2 += '	</div>';						
				divStr2 += '	<div class="arrow aMiddleRight">';
				divStr2 += '		<span class="out">◆</span>'
				divStr2 += '		<span class="inside">◆</span>'
				divStr2 += '	</div>';						
				divStr2 += '</div>';
				
			return this.each(function(){
				var _this = $(this);
				
				//随机生成一个id
				_this.uuid = 'J_' + new Date().getTime() + Math.ceil(Math.random()*1000);
				
				//参数列表
				var p = {
					boxEle: null,  
					eType: 'mouseover',
					content: _this.attr('data-tips') || '',
					dirct: 'auto',
					ajaxFn: false,
					spac:0
				};
				$.extend(true,p,options);
				
				//自设定box或默认tips
				var divType = p.boxEle ? "others" : "tips";
				
				//确认盒子模型
				_this.$boxId = p.boxEle ? ( typeof(p.boxEle) == 'string' ? $(p.boxEle) : p.boxEle ) : 
							( (p.eType == 'mouseover' || p.eType == 'focus') ? $(divStr) : $(divStr2) );
							
				switch(typeof p.content){
					case 'string':
						_this.content = p.content;
						break;
					case 'function':
						_this.content = p.content.call(_this);
						break;
					default:
						_this.content = '暂无数据';
						break;
				}
					
				//提供一个接口给ajax改变数据
				_this.setData = function(content){
					this.data("ajax",content);
				}
				
				_this.setHtml = function(content){
					this.$boxId.find('.content').html(content);
				}
				
					
				if(p.eType == 'mouseover'){
					
					_this.bind('mouseover',function(){
						if(divType == 'tips'){
							if($('body').find('#G_tips').length < 1){
								_this.$boxId.appendTo("body");
							}else{
								_this.$boxId = $('#G_tips');
							}	
						}
						
						clearTimeout(_this.timer);
						_this.timer = setTimeout(function(){
							$.openTips(_this,_this.$boxId,p.dirct,p.spac,_this.content);
						},300);
						
						return false;
					});
					
					_this.bind('mouseout',function(){
						clearTimeout(_this.timer);
						if(divType == 'tips'){
							_this.$boxId.hide();
							_this.$boxId.remove();

						}else{
							_this.$boxId.hide();
						}	
						return false;
					});
					
					_this.$boxId.bind('mouseover',function(){
						clearTimeout(_this.timer);
						$(this).show();
						return false;
					});
					
					_this.$boxId.bind('mouseout',function(){
						clearTimeout(_this.timer);
						if(divType == 'tips'){
							$(this).hide();
							$(this).remove();
						}else{
							$(this).hide();
						}	
						return false;
					});
					
				}else if(p.eType == 'focus'){
					
					_this.bind('focus',function(){
						if(divType == 'tips'){
							if($('body').find('#G_tips').length < 1){
								_this.$boxId.appendTo("body");
							}else{
								_this.$boxId = $('#G_tips');
							}	
						}
						
						clearTimeout(_this.timer);
						_this.timer = setTimeout(function(){
							$.openTips(_this,_this.$boxId,p.dirct,p.spac,_this.content);
						},100);
						
						return false;
					});
					
					_this.bind('blur',function(){
						clearTimeout(_this.timer);
						if(divType == 'tips'){
							_this.$boxId.hide();
							_this.$boxId.remove();
						}else{
							_this.$boxId.hide();
						}	
						return false;
					});
					
				}else if(p.eType == 'click'){
					
					_this.bind('click',function(){
						//如果uuid相等
						if(_this.$boxId.data('uuid') == _this.uuid){
							//如果是显示的，则隐藏并吧uuid设为空
							if(!_this.$boxId.is(":hidden")){
								_this.$boxId.hide();
								_this.$boxId.data('uuid','');
								return false;
							}	
						}
						
						//ajax方式
						if(p.ajaxFn && typeof p.ajaxFn == 'function'){
							//查看是否已经加载过
							if(_this.data("ajax")){
								_this.content = _this.data("ajax");
							}else{
								_this.content = '<p>正在加载...</p>';
								p.ajaxFn.call(_this);
							}	
						}
						
						if(divType == 'tips'){
							if($('body').find('#G_info').length < 1){
								_this.$boxId.appendTo("body");
							}else{
								_this.$boxId = $('#G_info');
							}
							//设定uuid
							_this.$boxId.data('uuid',_this.uuid);
						}
						
						$.openTips(_this,_this.$boxId,p.dirct,p.spac,_this.content);

						$(document).one('mousedown.tips', function(e){  
								
							var $target = $(e.target);
							if( (!$target.closest(_this.$boxId).length) 
								&& $target.get(0) !== _this.get(0)
								&& !$target.closest(_this).length 
							){	
								//停止ajax
								if(p.ajaxFn && typeof p.ajaxFn == 'function'){
									_this.ajaxGet.abort();
								}
								_this.$boxId.hide();				
								$(document).unbind('mousedown.tips');	
							}			
						});	
								
						return false;
					});
				}	
			});		
		}
	});
	
	//boxId=提示框，content=提示文本
	$.extend({
		openTips:function(srcId,boxId,dirct,spac,content){
			if(typeof(srcId) == 'undefined' || typeof(boxId) == 'undefined' || !srcId || !boxId){
				return;
			}
			//判断类型
			var $srcId = typeof(srcId) == 'object' ? (srcId instanceof jQuery ? srcId : $(srcId)) : $(srcId);
			var $boxId = typeof(boxId) == 'object' ? (boxId instanceof jQuery ? boxId : $(boxId)) : $(boxId);
			
			//默认间隙为3
			spac = spac ? spac : 5;
			
			//先植入html,后面才可以获取高度
			if(content){
				if($boxId.find(".content").length > 0){
					$boxId.find(".content").html(content);
				}else{
					$boxId.html(content);
				}	
			}
					
			var top = $srcId.offset().top,
			 	left = $srcId.offset().left,
			 	srcWidth = $srcId.outerWidth(),
			 	srcHeight = $srcId.outerHeight(),
				boxHeight = $boxId.outerHeight(),
				winHeight = $(window).height();
				
			//默认处理方向，固定的e.target来确定方向
			if(!dirct || dirct == "auto"){
				if(winHeight - top - srcHeight > boxHeight + spac){
					//如果是global_tips,则加上箭头
					if( $boxId.hasClass('global_tips') ){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border top-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle top-triangle");
					}else if( $boxId.hasClass('slide_info') ){
						$boxId.find(".arrow").removeClass().addClass("arrow aTopLeft");
					}
					$boxId.css('left',left + 'px');
					$boxId.css('bottom','auto');
				   	$boxId.css('top',top + srcHeight + spac + 'px');
				   	
				}else if(top > boxHeight + spac){
					if($boxId.hasClass('global_tips')){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border bottom-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle bottom-triangle");
					}else if( $boxId.hasClass('slide_info') ){
						$boxId.find(".arrow").removeClass().addClass("arrow aBottomLeft");
					}
					$boxId.css('left',left + 'px');
				    $boxId.css('top','auto');
				    $boxId.css('bottom',winHeight - top + spac +'px');
				    
				}else{
					if($boxId.hasClass('global_tips')){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border rightcenter-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle rightcenter-triangle");
					}else if( $boxId.hasClass('slide_info') ){
						$boxId.find(".arrow").removeClass().addClass("arrow aMiddleLeft");
					}
					$boxId.css('left',left + srcWidth + spac + 'px');
				    $boxId.css('top',top + (srcHeight/2) - (boxHeight/2) + 'px');
				    $boxId.css('bottom','auto');
				}	
			
			//指定为left,分为上下两种	
			}else if(dirct == "left"){
				//默认上下居中
				if(winHeight - top - srcHeight > boxHeight + spac){
					if($boxId.hasClass('global_tips')){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border rightm-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle rightm-triangle");
					}
					$boxId.css('left',left + srcWidth + spac + 'px');
				    $boxId.css('top',top + (srcHeight/2) - (boxHeight/2) + 'px');
				    $boxId.css('bottom','auto');
				//上
				}else if(top < (boxHeight/2)){
					if($boxId.hasClass('global_tips')){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border right-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle right-triangle");
					}
					$boxId.css('left',left + srcWidth + spac + 'px');
					$boxId.css('bottom','auto');
				   	$boxId.css('top',top + 'px');
				//下
				}else{
					if($boxId.hasClass('global_tips')){
						$boxId.find(".triangle-border").removeClass().addClass("triangle-border rightb-triangle-border");
				   		$boxId.find(".triangle").removeClass().addClass("triangle rightb-triangle");
					}
					$boxId.css('left',left + srcWidth + spac + 'px');
					$boxId.css('top','auto');
				    $boxId.css('bottom',winHeight - top - srcHeight + spac +'px');
				}
				
			}else if(dirct == "top"){
				//如果是global_tips,则加上箭头
				if( $boxId.hasClass('global_tips') ){
					$boxId.find(".triangle-border").removeClass().addClass("triangle-border top-triangle-border");
			   		$boxId.find(".triangle").removeClass().addClass("triangle top-triangle");
				}else if( $boxId.hasClass('slide_info') ){
					$boxId.find(".arrow").removeClass().addClass("arrow aTopLeft");
				}
				$boxId.css('left',left + 'px');
				$boxId.css('bottom','auto');
			   	$boxId.css('top',top + srcHeight + spac + 'px');
			}

			$boxId.setMaxIndex().show();
		}
	});
})(jQuery);

