<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>新建标签</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="cache-control" content="no-cache">
	<meta http-equiv="expires" content="0">    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/property/labelExport.js?${version}"></script>
	<script type="text/javascript" src="${ctx}/resources/script/property/export.js?${version}"></script>
	<script type="text/javascript">
		
		var ass_prop = {	
			"行业兴趣": [
						<c:forEach items="${industryList}" var="label" varStatus="status">
						{"id":${label.labelId},"title":'${label.labelName}'+"("+${label.num}+")","info":"行业兴趣","count":${label.num}}<c:if test="${status.last==false}">,</c:if>
						</c:forEach>
					],
					"社会身份": [
						<c:forEach items="${societyList}" var="label" varStatus="status">
						{"id":${label.labelId},"title":'${label.labelName}'+"("+${label.num}+")","info":"社会身份","count":${label.num}}<c:if test="${status.last==false}">,</c:if>
						</c:forEach>
					],
					"年龄段": [
						<c:forEach items="${ageList}" var="label" varStatus="status">
						{"id":${label.labelId},"title":'${label.labelName}'+"("+${label.num}+")","info":"年龄段","count":${label.num}}<c:if test="${status.last==false}">,</c:if>
						</c:forEach>
					],
					"性别": [
						<c:forEach items="${genderList}" var="label" varStatus="status">
						{"id":${label.labelId},"title":'${label.labelName}'+"("+${label.num}+")","info":"性别","count":${label.num}}<c:if test="${status.last==false}">,</c:if>
						</c:forEach>
					]
		};
	
		/*
		var ass_prop = {
		"行业兴趣": [
		{"id":1,"title":'游戏',"info":"行业兴趣","count":395980},
		{"id":10,"title":'医疗',"info":"行业兴趣","count":26881},
		{"id":11,"title":'美容纤体',"info":"行业兴趣","count":67669},
		{"id":12,"title":'媒体',"info":"行业兴趣","count":164673},
		{"id":13,"title":'3C',"info":"行业兴趣","count":38700},
		{"id":14,"title":'酒店旅游',"info":"行业兴趣","count":69688},
		{"id":15,"title":'移动业务',"info":"行业兴趣","count":269541},
		{"id":2,"title":'房地产',"info":"行业兴趣","count":107455},
		{"id":3,"title":'家电',"info":"行业兴趣","count":1131271},
		{"id":4,"title":'饮品',"info":"行业兴趣","count":17023},
		{"id":5,"title":'服饰',"info":"行业兴趣","count":750915},
		{"id":6,"title":'电商',"info":"行业兴趣","count":142700},
		{"id":7,"title":'金融',"info":"行业兴趣","count":275720},
		{"id":8,"title":'教育',"info":"行业兴趣","count":40332},
		{"id":9,"title":'汽车服务',"info":"行业兴趣","count":13441}
		],
		"社会身份": [
		{"id":16,"title":'学生',"info":"社会身份","count":0},
		{"id":17,"title":'白领',"info":"社会身份","count":0},
		{"id":18,"title":'公务员',"info":"社会身份","count":0},
		{"id":19,"title":'商旅人士',"info":"社会身份","count":0},
		{"id":20,"title":'居家人士',"info":"社会身份","count":0},
		{"id":21,"title":'农民工',"info":"社会身份","count":0}
		],
		"年龄段": [
		{"id":22,"title":'少年（18岁以下）',"info":"年龄段","count":0},
		{"id":23,"title":'青年（18~44岁）',"info":"年龄段","count":2},
		{"id":24,"title":'中年（45~59岁）',"info":"年龄段","count":5104},
		{"id":25,"title":'老年（60岁以上）',"info":"年龄段","count":0}
		],
		"性别": [
		{"id":26,"title":'男',"info":"性别","count":5104},
		{"id":27,"title":'女',"info":"性别","count":0}
		]
		};
		*/
		
		//包含标签数组
		var enTagVal=[];
		//排除标签数组
		var outTagVal=[];
	</script>
  </head>
  
  <body>
   		<div id="main" class="clearfix" style="">
   		<h1 class="title"><span class="f_r"><a  href="${ctx}/property/propertyIndex"><font color="blue">返回>></font>&nbsp;&nbsp;</a></span>属性数据导出&nbsp;<span></h1>
		<form id="export" autocomplete="OFF" action="${ctx}/label/statExportNum" method="post">
			<input type="hidden" name="token" value="${token}" />
			<input type="hidden" id="opt" name="opt" value="" />
			<input type="hidden" id="tagId" name="tagId" value="" />
            <table width="100%"  border="0" cellspacing="0" cellpadding="0" class="user_table m_t20">
                <tr class="mul_cate">
                    <td width="102" align="right">包含标签：</td>
                    <td width="10" class="red">*</td>
                    <td width="400">
                    	<textarea type="text" id="enTag" class="add_text" value="" style="width:380px; height:28px"></textarea>
                    	<input type="hidden" id="enTagId" name="includeTagIds" value="${labelId}" />
                    </td>
                   	<td>
                   		<span class="input_set" id="enSet">设置</span>
                   	</td>
                </tr>
                <tr>
                	<td colspan="2"></td>
                    <td colspan="2" valign="top">
                    	<label for="or"><input type="radio" class="df_input" id="or" name="inCnd" value="or" checked="checked" />&nbsp;并集&nbsp;&nbsp;</label>
                   		<label for="and"><input type="radio" class="df_input" id="and" name="inCnd" value="and" />&nbsp;交集&nbsp;&nbsp;</label>
                    </td>
                </tr>
                <tr class="mul_cate">
                    <td align="right">排除标签：</td>
                    <td></td>
                    <td>
                    	<textarea type="text" id="outTag" class="add_text" value="" style="width:380px; height:28px"></textarea>
                    	<input type="hidden" id="outTagId" name="excludeTagIds" value="" />
                    </td>
                   	<td>
                   		<span class="input_set" id="outSet">设置</span>
                   	</td>
                </tr>
                <tr>
                    <td align="right">活跃分值：</td>
                    <td></td>
                    <td>
                    	<select class="df_select" name="scores">
                    		<option value="0-99999999">不限</option>
							<option value="0-0">0分</option>
							<option value="1-10">1-10分</option>
							<option value="11-50">11-50分</option>
							<option value="51-100">51-100分</option>
							<option value="100-99999999">100分以上</option>
						</select>
                    </td>
                   	<td>
                   		&nbsp;
                   	</td>
                </tr>
				<tr>
                    <td align="right">活跃时间：</td>
                    <td></td>
                    <td>
                    	<select class="df_select" name="months">
                    		<option value="0">不限</option>
							<option value="1">最近一个月有过活跃行为</option>
							<option value="3" selected="selected">最近三个月有过活跃行为</option>
							<option value="6">最近半年有过活跃行为</option>
							<option value="12">最近一年有过活跃行为</option>
						</select>
                    </td>
                   	<td>
                   		&nbsp;
                   	</td>
                </tr>
                <tr>
                	<td colspan="2"></td>
            		<td colspan="2" class="head_td"><strong>当前收件人总数：<span id="personSum" class="red">0</span>人</strong><span class="f666">（未过滤去重）</span></td>
            	</tr>
			</table>
			<table width="100%" border="0" cellspacing="0" cellpadding="0" class="user_table m_t20">
                <tr>
                	<td width="102" align="right"></td>
                    <td width="10" class="red"></td>
                    <td>
                        <ul class="f_ul">
                            <li><input class="input_yes" id="export_btn" type="button" value="确 定" onclick="statNum()"/></li>
                        </ul>
                    </td>
                    <td></td>
                </tr>
            </table>
        </form>
	</div>
		
	<div class="searchBox searchHoriz" id="slideSearchProp" style="display:none">
		<div class="limiter" style="width:390px">
			<div class="pop_content">
				<div class="search_result"></div>
			</div>
		</div>
	</div>
	
	<div class="popBox searchBox" id="remindCustom" style="display:none;">
		<div class="pop_close">X</div>
		<div class="limiter" style="width:400px">
			<div class="pop_title">标题</div>
			<div class="pop_content">
				<div class="al_center">	
				</div>
				<div class="al_submit">
					<div id="div_ok" class='cmt_wrap'>
						<input class='input_yes' type="button" value='导出文件' onclick="downloadFile();" />
					</div>
				</div>
			</div>
		</div>
	</div>
	
	<form id="addPath" action="${ctx}/label/download" method="post">
		<input type='hidden' id="exportPath" name='exportPath'/>
		<input type='hidden' id="counter" name='counter'/>
	</form>
	<input type="hidden" name="msg" id="msg" value="${message}">
	<br/>
	<br/>
	<br/>
  </body>
</html>
