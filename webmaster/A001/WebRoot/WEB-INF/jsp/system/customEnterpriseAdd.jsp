<%@ page language="java" pageEncoding="UTF-8"%>
<%@ include file="/WEB-INF/jsp/common/tag.jsp"%> 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>logo定制</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="zh-cn" />
	<meta http-equiv="pragma" content="no-cache"/>
	<meta http-equiv="cache-control" content="no-cache"/>
	<meta http-equiv="expires" content="0"/>    
	
	<%@ include file="/WEB-INF/jsp/common/css.jsp"%> 
	<link rel="stylesheet" type="text/css" href="${ctx}/resources/css/uploader.css?${version}"/>
	<%@ include file="/WEB-INF/jsp/common/script.jsp"%> 
	<script type="text/javascript" src="${ctx}/resources/script/system/customEnterpriseAdd.js?${version}"></script>
    <script>

       $(function(){
	      $("input[type=file]").change(function(){$(this).parents(".uploader").find(".filename").val($(this).val());});
	      $("input[type=file]").each(function(){
	            if($(this).val()==""){
		            $(this).parents(".uploader").find(".filename").val("No file selected...");
		        }
	      });
       });
</script>
  
  </head>
  
  <body>
    <div id="main" class="clearfix" style="">
		<div class="ribbon clearfix"><h1 class="title"><!-- <span class="search_top f_r"><a class="search_hide" href="javascript:void(0)">查找</a></span> -->企业定制&nbsp;<span>${title}</span></h1></div>
		
		<form id="customForm"   name="customForm" method="post" enctype="multipart/form-data">	
		 <br/>		
		 <table width="100%" cellspacing="0" cellpadding="0" border="0"  class="imgTable">
		    <tr> 
		        <th>logo</th>
                <td> 
                  <table>
                     <tr>
                       <td width="20" align="left" ><input id="defaultLogo" type="radio" name="type" value="1" checked="checked"  /></td>
                       <td width="40" align="center"><span>默认</span></td>
                       <td align="right" width="100" rowspan="2"><div id="showLogo"><img src="${pageContext.request.contextPath}/resources/img/logo_blue7.png" /></div></td>
                     </tr>
                     <tr>
                       <td width="20" align="left" ><input id="newLogo" type="radio" name="type" value="2"/></td>
                       <td width="40" align="center"><span>定制</span></td>
                       <td align="right" width="100"></td>
                     </tr>
                  </table>
                </td> 
            </tr>
		    <tr>
		       <th width="20%"></th>
		      <td>
		      <div class="uploader green">
                  <input type="text" class="filename" readonly="readonly"/>
                  <input type="button" name="file" class="button" disabled="disabled" value="选择logo..."/>
                  <input type="file" size="30" class="button" disabled="disabled" name="uploadImg"/>
              </div>
              </td>
		    </tr>
		 </table>

		<br/>		
		<div class="ribbon clearfix"></div>
		<div class="ribbon clearfix"></div>
		<div class="ribbon clearfix"></div>
		<div class="ribbon clearfix"></div><br/>
		<table align="center">
			<tr>
				<td align="center">
					<span >
					  <input id="btn_save" type="button" class="input_yes" value="保 存" title="保存" onclick="subimtBtn()" />
					  <input type="button" class="input_no" value="关 闭" title="关闭" onclick="closeWin()"/>
				    </span>
				</td>
			</tr>
			<tr>
				<td align="center">
					<img style="display:none" id="prompt" src="${ctx}/resources/img/loading3.gif" />
				</td>
			</tr>
		</table>
		<input type="hidden" name="msg" id="msg" value="${message}" />
		<input type="hidden" name="token" value="${token}" />
		<input type="hidden" id="path" value="${pageContext.request.contextPath}" />
		<input type="hidden" name="ctx" id="ctx" value="${ctx}" />
	</form>
	</div>
  </body>
</html>
