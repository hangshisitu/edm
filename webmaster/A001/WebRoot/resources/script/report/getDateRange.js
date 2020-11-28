//获取日期范围
function getDateRange(range,startId,endId) {
	
	if(range=="") {
		$("#"+startId).val("");
		$("#"+endId).val("");
	}else {
		//获取时间间隔毫秒数
		var ms = 24*60*60*1000*range;
		
		//获取当前时间毫秒数
		var now = new Date().getTime();
		
		//获取开始时间
		var start = now - ms;
		var startDate = new Date();
		startDate.setTime(start);
		
		var year = startDate.getFullYear();
		var month = startDate.getMonth()+1;
		if(month<10) {
			month = "0"+month;
		}
		var day = startDate.getDate()
		if(day<10) {
			day = "0"+day
		}
		var startDateStr = year+"-"+month+"-"+day;
		$("#"+startId).val(startDateStr);
		
		//当前时间
		var endDate = new Date();
		
		year = endDate.getFullYear();
		month = endDate.getMonth()+1;
		if(month<10) {
			month = "0"+month;
		}
		day = endDate.getDate();
		if(day<10) {
			day = "0"+day
		}
		var endDateStr = year+"-"+month+"-"+day;
		$("#"+endId).val(endDateStr);
	}
}
