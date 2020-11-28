(function($){
    $.fn.UIselect = function(options){
        var defaults = {
            width:"auto",
			radio:1,   //是否单选  1为单选
			arrdata:"",  //二维数据组
			tit_id:"",   //标题
			id_id:"",    //值ID
			ho_fun:"", //列表点击函数
			html:"",   //直接植入html
			foot_html:"",  //底部html
			id:"",    //位于比较ID
			url:"",   //加载地址html
			edit_top:0, //缝隙
			align:"left",
			url_id:"",
			url_error:"",
			select_div:'.select_div',
			height:0,
			border:"1px solid #99a9ba"
  		}
       this.data('edit',$.extend(defaults, options));
       var $this=this;
       var $boxId=null;
       this.each(function(){
       	
			if($this.data('edit').id_id!=="")
			{
				if($($this.data('edit').id_id).val()!=="")
				{
					if($this.data('edit').tit_id=="")
					{
						var id2="#"+$this.attr("id");
					}
					else
					{
						var id2=$this.data('edit').tit_id;
					}
					var josn=$this.data('edit').arrdata;
					var id2_html="";
					var id_id_list=$($this.data('edit').id_id).val().split(",");
					
					for ( var i=0 ; i < josn.length ; ++i ) {
						for ( var ii=0 ; ii < id_id_list.length ; ++ii ) {
							if(id_id_list[ii]==josn[i][0])
							{
								if(id2_html=="")
								{
									id2_html+=josn[i][1];
								}
								else
								{
									id2_html+=","+josn[i][1];
								}
								
							}
						}
					}
					$(id2).html(id2_html);
					
				}
			}
	
			$(this).bind("click",function(){
					if($boxId != null){
						$boxId.hide();
					}
					//初始化
					$($this.data('edit').select_div).html("");
					$($this.data('edit').select_div).css("width","auto");
					$($this.data('edit').select_div).css("height","auto");
					$($this.data('edit').select_div).css("border",$this.data('edit').border);


					//下拉ID
					var old_id=$($this.data('edit').select_div).attr("id");
					var now_id=$(this).attr("id");
					var select_id="se_"+now_id;
					$($this.data('edit').select_div).attr("id",select_id);
					var id=$this.data('edit').id;
					if(id==""){id=$(this); }else{id=$(id);}
				  
					//验证复选框
					if($this.data('edit').url_id!==""){
						var checkedIds=""
							$("input[name="+$this.data('edit').url_id+"]").each(function() {
								if($(this).prop("checked") == true) {
									checkedIds += $(this).val() + ",";
								}
							});
							if(checkedIds=="")
							{
								error_message($this.data('edit').url_error,1);
								return false;
							}
							else
							{
								var Cts =$this.data('edit').url;

								if(Cts.indexOf("?") > 0 )
								{
									var opn_url=$this.data('edit').url+"&"+$this.data('edit').url_id+"="+checkedIds;
								}
								else
								{
									var opn_url=$this.data('edit').url+"?"+$this.data('edit').url_id+"="+checkedIds;
								}
							}
					}
					else
					{
						opn_url=$this.data('edit').url;
					}
					//数组单选
					if($this.data('edit').arrdata!==""){
						var josn=$this.data('edit').arrdata;
						var load_html="<ul>";
						var id1=$this.data('edit').id_id;
						if($this.data('edit').tit_id=="")
						{
							var id2="#"+$this.attr("id");
						}
						else
						{
							var id2=$this.data('edit').tit_id;
						}
						
						if(josn.length>0){
							if($this.data('edit').radio==1){
								for ( var i=0 ; i < josn.length ; ++i ) {
									load_html+="<li><a hidefocus=\"true\" onclick=\"categoryId_fun(\'"+id1+"\',\'"+josn[i][0]+"\',\'"+id2+"\',\'"+josn[i][1]+"\',\'"+$this.data('edit').ho_fun+"\')\" href=\"javascript:void(0)\" title=\"" + josn[i][1] + "\">"+josn[i][1]+"</a></li>";
								}
							}
							else
							{
								
								if($($this.data('edit').id_id).val()==""){
									var class_tit="checkbox_0";
									for ( var i=0 ; i < josn.length ; ++i ) {
										load_html+="<li  class=\"checkbox\" id=\""+$this.attr("id")+"_li_"+josn[i][0]+"\" ><a hidefocus=\"true\" onclick=\"checkbox_fun(\'"+id1+"\',\'"+josn[i][0]+"\',\'"+id2+"\',\'"+$this.attr("id")+"\',\'"+$this.data('edit').ho_fun+"\')\"  href=\"javascript:void(0)\" class=\""+class_tit+"\" >"+josn[i][1]+"</a></li>";						
									}
								}
								else
								{
									var id_id_list=$($this.data('edit').id_id).val().split(",");
									var class_tit="checkbox_0";
									for ( var i=0 ; i < josn.length ; ++i ) {
										class_tit="checkbox_0";									
										for ( var ii=0 ; ii < id_id_list.length ; ++ii ) {
											if(String(id_id_list[ii])==String(josn[i][0]))
											{
												class_tit="checkbox_2";
											}
										}
										load_html+="<li  class=\"checkbox\" id=\""+$this.attr("id")+"_li_"+josn[i][0]+"\" ><a hidefocus=\"true\" onclick=\"checkbox_fun(\'"+id1+"\',\'"+josn[i][0]+"\',\'"+id2+"\',\'"+$this.attr("id")+"\',\'"+$this.data('edit').ho_fun+"\')\"  href=\"javascript:void(0)\" class=\""+class_tit+"\" >"+josn[i][1]+"</a></li>";	
									}
									
								}
								
							}
						}else{
							load_html+="<li><p style='text-align:center'>没有数据！</p></li>";	
						}
						
						if($this.data('edit').foot_html!==""){
							load_html+=$this.data('edit').foot_html;
						}
						
						load_html+="</ul>";
						$($this.data('edit').select_div).html(load_html);
					}
					
					//加载html
					if($this.data('edit').html!==""){
						$($this.data('edit').select_div).html($this.data('edit').html);
					}
					
					
					//加载URL
					if($this.data('edit').url!==""){
						$.ajax({
						   type: "GET",
						   cache: false,
						   async: false,
						   url: opn_url,
						   success: function(msg){
							   $($this.data('edit').select_div).html(msg);
						   }
						});
					}
					
								
					var li_width=$($this.data('edit').select_div).width()-20;
					var select_div_height=$($this.data('edit').select_div).height();

					
					//宽度
					if($this.data('edit').width>0){
						$($this.data('edit').select_div+" li").css("width",$this.data('edit').width-20+"px"); 
						$($this.data('edit').select_div).css("width",$this.data('edit').width+"px");
					}
					else
					{
						$($this.data('edit').select_div+" li").css("width",li_width+"px"); 
						$($this.data('edit').select_div).css("width","auto");
					}
					
				  
				  
				   var top=id.offset().top+id.height()+2;
				   var left=id.offset().left;
				   var id_h=id.height();
				   if($(window).width()-left>$($this.data('edit').select_div).width() && $this.data('edit').align=="left")
				   {
					   $($this.data('edit').select_div).css("right","auto");
					  $($this.data('edit').select_div).css("left",left+"px");
				   }
				   else
				   {
					   $($this.data('edit').select_div).css("left","auto");
					   $($this.data('edit').select_div).css("right",$(window).width()-left-id.width()-2+"px");
				   }
				  
				  if($this.data('edit').edit_top>0)
				  {
					  top=top+$this.data('edit').edit_top;
				  }
				  
				  if($this.data('edit').height>0 && $this.data('edit').height<select_div_height)
				  {
					  select_div_height=$this.data('edit').height;
					   $($this.data('edit').select_div).css("height",$this.data('edit').height+"px");
				  }
				  
				  
				  if($(window).height()-top>select_div_height)
				  {
					  $($this.data('edit').select_div).css("top",top+"px");
					  $($this.data('edit').select_div).css("bottom","auto");
				  }
				  else if(id.position().top>select_div_height){
					  $($this.data('edit').select_div).css("top","auto");
					  $($this.data('edit').select_div).css("bottom",$(window).height()-top+id_h+2);
				  }else if(id.position().top>$(window).height()-top){
					  $($this.data('edit').select_div).css("height",id.position().top-50+"px");
					  $($this.data('edit').select_div).css("top","auto");
					  $($this.data('edit').select_div).css("bottom",$(window).height()-top+id_h+2);
				  }else{
					  $($this.data('edit').select_div).css("height",$(window).height()-top-50+"px");
					  $($this.data('edit').select_div).css("top",top+"px");
					  $($this.data('edit').select_div).css("bottom","auto");
				  }
				  
				 
					
				  if(old_id=="se_"+now_id)
				  {
					  $($this.data('edit').select_div).attr("id","");
					  $($this.data('edit').select_div).hide();
				  }
				  else
				  {
					  $($this.data('edit').select_div).show();
				  }
				  
				  $boxId=$($this.data('edit').select_div);
				  $(document).bind('mousedown.select', function(e){	   		 
						if(isParent(e.target, $boxId[0])==false && $(e.target).get(0) !== $this.get(0)){
							$boxId.attr("id","");
							$boxId.hide();
							$(document).unbind('mousedown.select');		 	
						}				
				  });	
				 return false;		
            });				
        });	
    };	
})(jQuery);
