$(document).ready(function() {
	// 日期查找
	var dates = $("#beginTime, #endTime").datepicker({
		numberOfMonths: 1,
		changeMonth: true,
		dateFormat: "yy-mm-dd",
		changeYear: true,
		onSelect: function(selectedDate) {
			if ($("#cycle_list").length > 0) {
				$("#cycle_list").html("时间选择");
			}
			var option = this.id == "beginTime" ? "minDate" : "maxDate", 
					instance = $(this).data("datepicker"), 
					date = $.datepicker.parseDate(instance.settings.dateFormat || $.datepicker._defaults.dateFormat, selectedDate, instance.settings
			);
			dates.not(this).datepicker("option", option, date);
		}
	});

	// 周期选择
	$("#cycle_list").UIselect({
		width: 118,
		id_id: "#cycle",
		arrdata: new Array([0, "时间选择"], [6, "近一周"], [29, "近一月"], [90, "近一季度"], [364, "近一年"]),
		ho_fun: "select_data()"
	});
});

function select_data() {
	if ($("#cycle").val() > 0) {
		$("#beginTime").datepicker("setDate", "-" + $("#cycle").val() + "d");
		$("#endTime").datepicker("setDate", "-0d");
	} else {
		$("#beginTime").val("");
		$("#endTime").val("");
	}
}
