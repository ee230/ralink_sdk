<html>
<head>
<title>Virtual Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

var MAX_RULES = 32;
var rules_num = <% getPortForwardRuleNumsASP(); %> ;
var rules_num_singleport = <% getSinglePortForwardRuleNumsASP(); %> ;

function deleteClick()
{
    return true;
}

function checkRange(str, num, min, max)
{
    d = atoi(str,num);
    if(d > max || d < min)
        return false;
    return true;
}

function checkIpAddr(field)
{
    if(field.value == ""){
        alert("Error. IP address is empty.");
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        alert('It should be a [0-9] number.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        alert('IP format error.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

   return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function formCheck()
{
	if(rules_num >= (MAX_RULES) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}

	if(!document.portForward.portForwardEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(	document.portForward.ip_address.value == "" &&
		document.portForward.fromPort.value == "" &&
		document.portForward.toPort.value   == "" &&
		document.portForward.comment.value  == "")
		return true;

	// exam IP address
    if(document.portForward.ip_address.value == ""){
        alert("Not set a ip address.");
        document.portForward.ip_address.focus();
        return false;
    }

    if(! checkIpAddr(document.portForward.ip_address) ){
        alert("IP address format error.");
        document.portForward.ip_address.focus();
        return false;
    }

    if(isAllNum(document.portForward.ip_address.value) == 0){
        alert("Invalid ip address.");
        document.portForward.ip_address.focus();
        return false;
    }

	// exam Port
	if(document.portForward.fromPort.value == ""){
		alert("Not set a port range.");
		document.portForward.fromPort.focus();
		return false;
	}

	if(isAllNum( document.portForward.fromPort.value ) == 0){
		alert("Invalid port number.");
		document.portForward.fromPort.focus();
		return false;
	}

	d1 = atoi(document.portForward.fromPort.value, 1);
	if(d1 > 65535 || d1 < 1){
		alert("Invalid port number!");
		document.portForward.fromPort.focus();
		return false;
	}
	
	if(document.portForward.toPort.value != ""){
		if(isAllNum( document.portForward.toPort.value ) == 0){
			alert("Invalid port number.");
			document.portForward.toPort.focus();
			return false;
		}
		d2 = atoi(document.portForward.toPort.value, 1);
		if(d2 > 65535 || d2 < 1){
			alert("Invalid port number.");
			document.portForward.toPort.focus();
			return false;
		}
		if(d1 > d2){
			alert("Invalid port range setting.");
			document.portForward.fromPort.focus();
			return false;
		}
   }
   return true;
}

function singlePortFormCheck()
{
	if(rules_num_singleport >= (MAX_RULES) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}

	if(!document.singlePortForward.singlePortForwardEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(	document.singlePortForward.ip_address.value == "" &&
		document.singlePortForward.publicPort.value == "" &&
		document.singlePortForward.privatePort.value   == "" &&
		document.singlePortForward.comment.value  == "")
		return true;

	// exam IP address
    if(document.singlePortForward.ip_address.value == ""){
        alert("Not set a ip address.");
        document.singlePortForward.ip_address.focus();
        return false;
    }

    if(! checkIpAddr(document.singlePortForward.ip_address) ){
        alert("IP address format error.");
        document.singlePortForward.ip_address.focus();
        return false;
    }

    if(isAllNum(document.singlePortForward.ip_address.value) == 0){
        alert("Invalid ip address.");
        document.singlePortForward.ip_address.focus();
        return false;
    }

	// exam public port
	if(document.singlePortForward.publicPort.value == ""){
		alert("Not set a public port.");
		document.singlePortForward.publicPort.focus();
		return false;
	}

	if(isAllNum( document.singlePortForward.publicPort.value ) == 0){
		alert("Invalid port number.");
		document.singlePortForward.publicPort.focus();
		return false;
	}

	d1 = atoi(document.singlePortForward.publicPort.value, 1);
	if(d1 > 65535 || d1 < 1){
		alert("Invalid port number!");
		document.singlePortForward.publicPort.focus();
		return false;
	}
		
	//exam private port
	if(document.singlePortForward.privatePort.value == ""){
		alert("Not set a private port.");
		document.singlePortForward.privatePort.focus();
		return false;
	}

	if(isAllNum( document.singlePortForward.privatePort.value ) == 0){
		alert("Invalid port number.");
		document.singlePortForward.privatePort.focus();
		return false;
	}

	d2 = atoi(document.singlePortForward.privatePort.value, 1);
	if(d2 > 65535 || d2 < 1){
		alert("Invalid port number!");
		document.singlePortForward.privatePort.focus();
		return false;
	}
	
   return true;
}


function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	var e = document.getElementById("forwardTitle");
	e.innerHTML = _("forward title");
	e = document.getElementById("forwardIntroduction");
	e.innerHTML = _("forward introduction");
	
	e = document.getElementById("forwardVirtualSrv");
	e.innerHTML = _("forward virtual server");
	e = document.getElementById("forwardVirtualSrvSet");
	e.innerHTML = _("forward virtual server setting");
	e = document.getElementById("forwardVirtualSrvDisable");
	e.innerHTML = _("firewall disable");
	e = document.getElementById("forwardVirtualSrvEnable");
	e.innerHTML = _("firewall enable");
	e = document.getElementById("forwardVirtualSrvIPAddr");
	e.innerHTML = _("forward virtual server ipaddr");
	e = document.getElementById("forwardVirtualSrvPortRange");
	e.innerHTML = _("forward virtual server port range");
	e = document.getElementById("forwardVirtualSrvProtocol");
	e.innerHTML = _("firewall protocol");
	e = document.getElementById("forwardVirtualSrvComment");
	e.innerHTML = _("firewall comment");
	e = document.getElementById("forwardVirtualSrvApply");
	e.value = _("firewall apply");
	e = document.getElementById("forwardVirtualSrvReset");
	e.value = _("firewall reset");

	e = document.getElementById("forwardCurrentVirtualSrv");
	e.innerHTML = _("forward current virtual server");
	e = document.getElementById("forwardCurrentVirtualSrvNo");
	e.innerHTML = _("firewall no");
	e = document.getElementById("forwardCurrentVirtualSrvIP");
	e.innerHTML = _("forward virtual server ipaddr");
	e = document.getElementById("forwardCurrentVirtualSrvPort");
	e.innerHTML = _("forward virtual server port range");
	e = document.getElementById("forwardCurrentVirtualSrvProtocol");
	e.innerHTML = _("firewall protocol");
	e = document.getElementById("forwardCurrentVirtualSrvComment");
	e.innerHTML = _("firewall comment");
	e = document.getElementById("forwardCurrentVirtualSrvDel");
	e.value = _("firewall del select");
	e = document.getElementById("forwardCurrentVirtualSrvReset");
	e.value = _("firewall reset");

	e = document.getElementById("singlePortVirtualSrv");
	e.innerHTML = _("forward single port virtual server");
	e = document.getElementById("singlePortVirtualSrvSet");
	e.innerHTML = _("forward single port virtual server setting");
	e = document.getElementById("singlePortVirtualSrvDisable");
	e.innerHTML = _("firewall disable");
	e = document.getElementById("singlePortVirtualSrvEnable");
	e.innerHTML = _("firewall enable");
	e = document.getElementById("singlePortVirtualSrvIPAddr");
	e.innerHTML = _("forward virtual server ipaddr");
	e = document.getElementById("singlePortVirtualSrvPublicPort");
	e.innerHTML = _("forward virtual server public port");
	e = document.getElementById("singlePortVirtualSrvPrivatePort");
	e.innerHTML = _("forward virtual server private port");
	e = document.getElementById("singlePortVirtualSrvProtocol");
	e.innerHTML = _("firewall protocol");
	e = document.getElementById("singlePortVirtualSrvComment");
	e.innerHTML = _("firewall comment");
	e = document.getElementById("singlePortVirtualSrvApply");
	e.value = _("firewall apply");
	e = document.getElementById("singlePortVirtualSrvReset");
	e.value = _("firewall reset");

	e = document.getElementById("singlePortCurrentVirtualSrv");
	e.innerHTML = _("forward current single port virtual server");
	e = document.getElementById("singlePortCurrentVirtualSrvNo");
	e.innerHTML = _("firewall no");
	e = document.getElementById("singlePortCurrentVirtualSrvIP");
	e.innerHTML = _("forward virtual server ipaddr");
	e = document.getElementById("singlePortCurrentVirtualSrvPublicPort");
	e.innerHTML = _("forward virtual server public port");
	e = document.getElementById("singlePortCurrentVirtualSrvPrivatePort");
	e.innerHTML = _("forward virtual server private port");
	e = document.getElementById("singlePortCurrentVirtualSrvProtocol");
	e.innerHTML = _("firewall protocol");
	e = document.getElementById("singlePortCurrentVirtualSrvComment");
	e.innerHTML = _("firewall comment");
	e = document.getElementById("singlePortCurrentVirtualSrvDel");
	e.value = _("firewall del select");
	e = document.getElementById("singlePortCurrentVirtualSrvReset");
	e.value = _("firewall reset");
}


function singlePortChange()
{
    if(! rules_num_singleport ){
 		disableTextField(document.singlePortForwardDelete.deleteSelSinglePortForward);
 		disableTextField(document.singlePortForwardDelete.reset);
	}else{
        enableTextField(document.singlePortForwardDelete.deleteSelSinglePortForward);
        enableTextField(document.singleortForwardDelete.reset);
	}


	if(document.singlePortForward.singlePortForwardEnabled.options.selectedIndex == 1){
		enableTextField(document.singlePortForward.ip_address);
		enableTextField(document.singlePortForward.publicPort);
		enableTextField(document.singlePortForward.privatePort);
		enableTextField(document.singlePortForward.protocol);
		enableTextField(document.singlePortForward.comment);
	}else{
		disableTextField(document.singlePortForward.ip_address);
		disableTextField(document.singlePortForward.publicPort);
		disableTextField(document.singlePortForward.privatePort);
		disableTextField(document.singlePortForward.protocol);
		disableTextField(document.singlePortForward.comment);
	}

}


function updateState()
{
	initTranslation();
    if(! rules_num ){
 		disableTextField(document.portForwardDelete.deleteSelPortForward);
 		disableTextField(document.portForwardDelete.reset);
	}else{
        enableTextField(document.portForwardDelete.deleteSelPortForward);
        enableTextField(document.portForwardDelete.reset);
	}
	
    	if(document.portForward.portForwardEnabled.options.selectedIndex == 1){
		enableTextField(document.portForward.ip_address);
		enableTextField(document.portForward.fromPort);
		enableTextField(document.portForward.toPort);
		enableTextField(document.portForward.protocol);
		enableTextField(document.portForward.comment);
	}else{
		disableTextField(document.portForward.ip_address);
		disableTextField(document.portForward.fromPort);
		disableTextField(document.portForward.toPort);
		disableTextField(document.portForward.protocol);
		disableTextField(document.portForward.comment);
	}

	singlePortChange();
}

</script>
</head>


                         <!--     body - Port Forwarding    -->
<body onload="updateState()">
<table class="body"><tr><td>
<h1 id="forwardTitle">Virtual Server  Settings </h1>
<% checkIfUnderBridgeModeASP(); %>
<p id="forwardIntroduction"> You may setup Virtual Servers to provide services on Internet.</p>
<hr />

<form method=post name="portForward" action=/goform/portForward>
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="forwardVirtualSrv">Port Forwarding</td>
</tr>
<tr>
	<td class="head" id="forwardVirtualSrvSet">
		Port Forwarding
	</td>
	<td>
	<select onChange="updateState()" name="portForwardEnabled" size="1">
	<option value=0 <% getPortForwardEnableASP(0); %> id="forwardVirtualSrvDisable">Disable</option>
    <option value=1 <% getPortForwardEnableASP(1); %> id="forwardVirtualSrvEnable">Enable</option>
    </select>
    </td>
</tr>


<tr>
	<td class="head" id="forwardVirtualSrvIPAddr">
		IP Address
	</td>
	<td>
  		<input type="text" size="16" name="ip_address">
	</td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvPortRange">
		Port Range
	</td>
	<td>
  		<input type="text" size="5" name="fromPort">-<input type="text" size="5" name="toPort"> &nbsp;&nbsp;
	</td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvProtocol">
		Protocol
	</td>
	<td>
		<select name="protocol">
   		<option select value="TCP&UDP">TCP&UDP</option>
		<option value="TCP">TCP</option>
   		<option value="UDP">UDP</option>
   		</select>&nbsp;&nbsp;
	</td>
</tr>

<tr>
	<td class="head" id="forwardVirtualSrvComment">
		Comment
	</td>
	<td>
		<input type="text" name="comment" size="16" maxlength="32">
	</td>
</tr>
</table>
<script>
    document.write("(The maximum rule count is "+ MAX_RULES +".)");
</script>
<p>
	<input type="submit" value="Apply" id="forwardVirtualSrvApply" name="addFilterPort" onClick="return formCheck()"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="forwardVirtualSrvReset" name="reset">
</p>
</form>

<br>
<hr />
<!--  delete rules -->
<form action=/goform/portForwardDelete method=POST name="portForwardDelete">

<table width="400" border="1" cellpadding="2" cellspacing="1">	
	<tr>
		<td class="title" colspan="5" id="forwardCurrentVirtualSrv">Current Port Forwarding in system: </td>
	</tr>

	<tr>
		<td id="forwardCurrentVirtualSrvNo"> No.</td>
		<td align=center id="forwardCurrentVirtualSrvIP"> IP Address</td>
		<td align=center id="forwardCurrentVirtualSrvPort"> Port Range</td>
		<td align=center id="forwardCurrentVirtualSrvProtocol"> Protocol</td>
		<td align=center id="forwardCurrentVirtualSrvComment"> Comment</td>
	</tr>

	<% showPortForwardRulesASP(); %>
</table>
<br>

<input type="submit" value="Delete Selected" id="forwardCurrentVirtualSrvDel" name="deleteSelPortForward" onClick="return deleteClick()">&nbsp;&nbsp;
<input type="reset" value="Reset" id="forwardCurrentVirtualSrvReset" name="reset">
</form>

<br>
<hr />
                  <!--------   body - Virtual Sever    --------->
<form method=post name="singlePortForward" action=/goform/singlePortForward>
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="singlePortVirtualSrv">Virtual Server</td>
</tr>
<tr>
	<td class="head" id="singlePortVirtualSrvSet">
		Virtual Server
	</td>
	<td>
	<select onChange="singlePortChange()" name="singlePortForwardEnabled" size="1">
	<option value=0 <% getSinglePortForwardEnableASP(0); %> id="singlePortVirtualSrvDisable">Disable</option>
    <option value=1 <% getSinglePortForwardEnableASP(1); %> id="singlePortVirtualSrvEnable">Enable</option>
    </select>
    </td>
</tr>


<tr>
	<td class="head" id="singlePortVirtualSrvIPAddr">
		IP Address
	</td>
	<td>
  		<input type="text" size="16" name="ip_address">
	</td>
</tr>

<tr>
	<td class="head" id="singlePortVirtualSrvPublicPort">
		Public Port
	</td>
	<td>
  		<input type="text" size="5" name="publicPort">
	</td>
</tr>

<tr>
        <td class="head" id="singlePortVirtualSrvPrivatePort">
                Private Port
        </td>
        <td>
                <input type="text" size="5" name="privatePort">
        </td>
</tr>

<tr>
	<td class="head" id="singlePortVirtualSrvProtocol">
		Protocol
	</td>
	<td>
		<select name="protocol">
   		<option select value="TCP&UDP">TCP&UDP</option>
		<option value="TCP">TCP</option>
   		<option value="UDP">UDP</option>
   		</select>&nbsp;&nbsp;
	</td>
</tr>

<tr>
	<td class="head" id="singlePortVirtualSrvComment">
		Comment
	</td>
	<td>
		<input type="text" name="comment" size="16" maxlength="32">
	</td>
</tr>
</table>
<script>
    document.write("(The maximum rule count is "+ MAX_RULES +".)");
</script>
<p>
	<input type="submit" value="Apply" id="singlePortVirtualSrvApply" name="addSinglePort" onClick="return singlePortFormCheck()"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="singlePortVirtualSrvReset" name="reset">
</p>
</form>

<br>
<hr />
<!--  delete rules -->
<form action=/goform/singlePortForwardDelete method=POST name="singlePortForwardDelete">

<table width="400" border="1" cellpadding="2" cellspacing="1">	
	<tr>
		<td class="title" colspan="6" id="singlePortCurrentVirtualSrv">Current Virtual Servers in system: </td>
	</tr>

	<tr>
		<td id="singlePortCurrentVirtualSrvNo"> No.</td>
		<td align=center id="singlePortCurrentVirtualSrvIP"> IP Address</td>
		<td align=center id="singlePortCurrentVirtualSrvPublicPort"> Public Port</td>
		<td align=center id="singlePortCurrentVirtualSrvPrivatePort"> Private Port</td>
		<td align=center id="singlePortCurrentVirtualSrvProtocol"> Protocol</td>
		<td align=center id="singlePortCurrentVirtualSrvComment"> Comment</td>
	</tr>

	<% showSinglePortForwardRulesASP(); %>
</table>
<br>

<input type="submit" value="Delete Selected" id="singlePortCurrentVirtualSrvDel" name="deleteSelSinglePortForward" onClick="return deleteClick()">&nbsp;&nbsp;
<input type="reset" value="Reset" id="singlePortCurrentVirtualSrvReset" name="reset">
</form>


			<!-----    end 2   ----->

</td></tr></table>
</body>
</html>
