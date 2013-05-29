<html><head><title>System Log</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");


var http_request = false;
function makeRequest(url, content) {
    http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert('Giving up :( Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = alertContents;
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() {
    if (http_request.readyState == 4) {
        if (http_request.status == 200) {
			uploadLogField(http_request.responseText);
        } else {
            alert('There was a problem with the request.');
        }
    }
}

function uploadLogField(str)
{
	if(str == "-1"){
		document.getElementById("syslog").value =
		"Not support.\n(Busybox->\n  System Logging Utilitie ->\n    syslogd\n    Circular Buffer\n    logread"
	}else
		document.getElementById("syslog").value = str;
}

function updateLog()
{
	makeRequest("/goform/syslog", "n/a", false);
}

function initTranslation()
{
	var e = document.getElementById("syslogTitle");
	e.innerHTML = _("syslog title");
	e = document.getElementById("syslogIntroduction");
	e.innerHTML = _("syslog introduction");

	e = document.getElementById("syslogSysLog");
	e.innerHTML = _("syslog system log");
	e = document.getElementById("syslogSysLogClear");
	e.value = _("syslog clear");
	e = document.getElementById("syslogSysLogRefresh");
	e.value = _("syslog refresh");
}

function pageInit()
{
	initTranslation();
	updateLog();
}

function clearlogclick()
{
	document.getElementById("syslog").value = "";
	return ture;
}

function refreshlogclick()
{
	updateLog();
	return true;
}

</script>

</head>
<body onload="pageInit()">
<table class="body"><tr><td>
<h1 id="syslogTitle">System Log</h1>
<p id="syslogIntroduction"> Syslog: </p>

<form method="post" name ="SubmitClearLog" action="/goform/clearlog">
	<input type="button" value="Refresh" id="syslogSysLogRefresh" name="refreshlog" onclick="refreshlogclick();">
	<input type="submit" value="Clear" id="syslogSysLogClear" name="clearlog" onclick="clearlogclick();">
</form>

<!-- ================= System log ================= -->
<table border="1" cellpadding="2" cellspacing="1" width="95%">
<tbody>
<tr>
	<td class="title"colspan="2" id="syslogSysLog">System Log: </td>
</tr>
<tr><td>
		<textarea style=font-size:9pt name="syslog" id="syslog" cols="80" rows="50" wrap="off" readonly="1">
		</textarea>
	</td>
</tr>
</table>


<br>
</td></tr></table>
</body></html>
