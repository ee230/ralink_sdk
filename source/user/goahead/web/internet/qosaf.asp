<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title> </title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("internet");


function initTranslation()
{
	var e;
	e = document.getElementById("QoSAFDirStr");
	e.innerHTML = _("qos AF Dir");
	e = document.getElementById("QoSAFNameStr");
	e.innerHTML = _("qos AF Name");
	e = document.getElementById("QoSAFRateStr");
	e.innerHTML = _("qos Rate");
	e = document.getElementById("QoSAFCeilStr");
	e.innerHTML = _("qos Ceil");
/*	e = document.getElementById("QoSAFUploadBandwidth");
	e.innerHTML = _("qos AF Upload Bandwidth");*/
	e = document.getElementById("QoSAFModifyStr");
	e.value = _("qos group modify");
}
	
function initValue()
{
	initTranslation();

    document.afattr.af_index.value = opener.document.forms[0].ModifyAFIndex.value;
    document.afattr.af_dir.value  = opener.document.forms[0].ModifyAFDir.value;
    document.afattr.af_name.value = opener.document.forms[0].ModifyAFName.value;
    document.afattr.af_rate.value = opener.document.forms[0].ModifyAFRate.value;
    document.afattr.af_ceil.value = opener.document.forms[0].ModifyAFCeil.value;
}


function checkForm()
{
	if(	document.afattr.af_name.value == "" ||
		document.afattr.af_rate.value == "" || 
		document.afattr.af_ceil.value == "" ){
		alert("Please fill the every field.");
		return false;
	}

	var rate = parseInt(document.afattr.af_rate.value);
	var ceil = parseInt(document.afattr.af_ceil.value);

	if( ceil > 100){
		alert("The ceil number format is bigger than 100 .");
		return false;
	}

	if( ceil < 0 || rate < 0  ){
		alert("The value can't be negative.");
		return false;
	}

	if( rate > ceil){
		alert("The rate can't be bigger than ceil.");
		return false;
	}

	return true;
}



function submit_form()
{
	if (checkForm() == true){
		document.afattr.submit();
		opener.location.reload();
		window.close();
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>


<form method=post name="afattr" action="/goform/QoSAFAttribute">
<table width="540" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">

  <input type=hidden name="af_index" id="af_index" value="">
  <tr>
	<td colspan="2" class="title" id="AFModifyTitle">
		<script>
			if(opener.document.forms[0].ModifyAFName.value == "")
				document.write("NoName");
			else
				document.write(opener.document.forms[0].ModifyAFName.value);
		</script>
	</td>
  </tr>

  <tr>
    <td class="head" id="QoSAFDirStr"> Name</td>
    <td><input type=text name="af_dir" maxlength=32 readonly></td>
  </tr>
  <tr>
    <td class="head" id="QoSAFNameStr"> Name</td>
    <td><input type=text name="af_name" maxlength=32></td>
  </tr>
  <tr>
    <td class="head" id="QoSAFRateStr">Rate</td>
    <td id="QoSAFUploadBandwidth"><input type=text name="af_rate" maxlength=2> % of bandwidth</td>
  </tr>
  <tr>
    <td class="head" id="QoSAFCeilStr">Ceil</td>
    <td><input type=text name="af_ceil" maxlength=3> % of bandwidth</td>
  </tr>
</table>
<input type="submit" id="QoSAFModifyStr" name="modify" value="modify" onClick="return checkForm()">

</form>

</td></tr></table>

</body>
</html>
