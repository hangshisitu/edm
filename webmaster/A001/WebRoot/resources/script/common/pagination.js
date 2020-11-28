//分页查询
function paginationQuery(flag) {
    var value = parseInt($("#currentPage").val());
    if(flag=="First") {
        $("#currentPage").val(1);//首页
    }else if(flag=="Previous") {
        $("#currentPage").val(value-1);//上一页
    }else if(flag=="Next") {
        $("#currentPage").val(value+1);//下一页
    }else if(flag=="Last") {
        $("#currentPage").val($("#totalPageCount").val());//尾页
    }else if(flag=="go") {
    	var jumpPageNo = $("#jumpPageNo").val();
    	if(jumpPageNo=="") {
    		alert("请输入页码!");
    		$("#jumpPageNo").focus();
    		return;
    	}else {
    		if(jumpPageNo>parseInt($("#totalPageCount").val())) {
    			jumpPageNo = parseInt($("#totalPageCount").val());
    		}else if(jumpPageNo<=0) {
    			jumpPageNo = 1;
    		}
    		$("#currentPage").val(jumpPageNo);
    	}
    }
    $("#ListForm").submit();
}