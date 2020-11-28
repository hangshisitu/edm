;(function($){
	$.fn.extend({
		Mselect:function(options){
			var defaults = {
				textId:'#' + this.attr('id'), /*默认打开源为本身*/
				valueId:'',                  /*放值元素的id*/
				boxId:'',
				json:'',
				createFn:$.fn.Mselect.createSelect,  /*提供外用的搜索函数*/
				itemFn:'',   //回调函数
				subArr:'',
				outCate:'',  //排除类别
				initFn:$.fn.Mselect.initInput
			};
			
			var p = $.extend({},defaults,options);
			
			function initInput(){
				p.initFn(p.textId,p.valueId,p.json,p.subArr);
			}
			
			initInput();
				
			return this.each(function(){
				var $this = $(this);
				//提供一个接口改变初始值
				$this.data('initInput',initInput);
				var $textId = $(p.textId);
				var $boxId = $(p.boxId);
				//促成动态改变数据源
				$this.data('json',p.json);
	
				$this.bind('click',function(){
					var json = $this.data('json');
					$(".search_result",$boxId).html("");
					
					//创建搜索
					p.createFn(json,p.itemFn,p.boxId,p.outCate,p.valueId);
					$.fn.Mselect.posfix($textId,$boxId);	
					$boxId.setMaxIndex().show();		
					
					$(document).bind('mousedown.Mselect', function(e){  		
						if(!($(e.target).closest($boxId).length || 
							$(e.target).get(0) === $this.get(0) ||
							$(e.target).get(0) === $textId.get(0)
						)){
							$boxId.hide();	
							$(document).unbind('mousedown.Mselect');	
						}			
					});				
					return false;
				})
			});		
		}		
	});	
	
	//提供对外的定位方法
	$.fn.Mselect.posfix = function($textId,$boxId){
		$('.search_result',$boxId).css({'height':"auto"});
		var top = $textId.offset().top,
		 	left = $textId.offset().left,
		 	srcWidth = $textId.outerWidth(),
		 	srcHeight = $textId.outerHeight(),
			boxHeight = $boxId.outerHeight(),
			winHeight = $(window).height();
		
	    if(winHeight - top - srcHeight > boxHeight){
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
	
	//公共方法二维下拉选择
	$.fn.Mselect.createSelect = function(json,fn,boxId,outCate,valueId){
		//排除其它下拉已选的链接
		outCate = [];
		$('.trig_list_id').each(function(){
			//先把本身已选的id排除
			if(valueId != "#"+$(this).attr('id')){
				outCate = outCate.concat($(this).val().split(','));
			}	
		});
	    var outCateStr = (','+ outCate.join(',') +',') || '';
	    var selfIdStr = (','+ $(valueId).val().split(',').join(',') +',') || '';
		var $boxId = $(boxId);
		var $resultId = $boxId.find(".search_result");
		$resultId.addClass('trig_result');
		var html = "";
		if(typeof json.length == 'number' && json.length > 0){
			var obj = json;	
			html += "<ul class='ul_result'>";
			for(i=0;i<obj.length;i++){
				//先排除不显示的
				if( outCateStr.indexOf(','+ obj[i].id +',') > -1 ){
					continue;
				}
				//是否选中
				if( selfIdStr.indexOf(','+ obj[i].id +',') > -1 ){
					html+="<li class=\"selected\"><label>";
					html+="<input type=\"checkbox\" name=\"trigchecks\" checked onclick=\""+ fn +";\">";
				}else{
					html+="<li><label>";
					html+="<input type=\"checkbox\" name=\"trigchecks\" onclick=\""+ fn +";\">";
				}
				html+="<span  title=\""+ obj[i].title +"\"";
				for(var key in obj[i]){
					if(key!="title"){
						html+=" data-"+ key +"=\""+ obj[i][key] +"\"";
					}	
				}
				html+=">" + obj[i].title + "</span>";
				html+="</label></li>";
			}
			html += "</ul>";
		}
		
		//先清空内容
		if(html){
			$resultId.html("").append(html).scrollTop(0);
		}else{
			$resultId.html("<p class='center'>暂无选项！</p>").scrollTop(0);
		}
		
			
	}
	
	$.fn.Mselect.initInput = function(textId,valueId,json,subArr){
			var $textId = (textId instanceof jQuery) ? textId : $(textId);
			var $valueId = (valueId instanceof jQuery) ? valueId : $(valueId);	
			var selfIdArr = $valueId.val().split(',') || '';
			var tempArr = [];
			var textArr = [];
			
			if(typeof json.length == 'number'){
				//删除不存在json的valueId
				for(i=0;i<json.length;i++){
					for(k=0;k<selfIdArr.length;k++){
						//遍历json查找已选的id
						if( selfIdArr[k] == json[i].id ){
							tempArr.push(json[i].id);
						}
					}
				}
				$valueId.val(tempArr.join(','));
				selfIdArr = $valueId.val().split(',');
				//如果一个链接都没有将删除整个选区value值
				if(tempArr.length < 1){
					$textId.parents('table').find(':input').val('');
				};
				
				for(i=0;i<json.length;i++){
					for(k=0;k<selfIdArr.length;k++){
						//遍历json查找已选的id
						if( selfIdArr[k] == json[i].id ){
							textArr.push(json[i].title);
							tempArr.push(json[i].id);
							
							if(typeof subArr == 'object'){
								var newarr = new Array(json[i].id,json[i].title);
								//过滤去重,自动增减
								var flag = false;
								for(j=0;j<subArr.length;j++){
									if(json[i].id == subArr[j].id){
										subArr.splice(j,1);
										flag = true;
										break;
									}
								}
								
								if(!flag){subArr.push(newarr)};
							}	
						}
					}		
				}
				
				if(textArr.length > 0){
					$textId.val('已选中' + textArr.length + '个选项！');
				}else{
					$textId.val('');
				}
			}
				
		}

})(jQuery);