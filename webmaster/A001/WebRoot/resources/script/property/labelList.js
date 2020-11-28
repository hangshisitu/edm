function selectCategory(url) {
	var id = $("#categoryId").val();
	window.location.href=url+"?categoryId="+id;
}


function importLabel(url) {
	window.location.href=url;
} 