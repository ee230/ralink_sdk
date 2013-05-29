<html><head><title>Statistic</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

function initTranslation()
{
	var e = document.getElementById("statisticTitle");
	e.innerHTML = _("statistic title");
	e = document.getElementById("statisticIntroduction");
	e.innerHTML = _("statistic introduction");

	e = document.getElementById("statisticMM");
	e.innerHTML = _("statistic memory");
	e = document.getElementById("statisticMMTotal");
	e.innerHTML = _("statistic memory total");
	e = document.getElementById("statisticMMLeft");
	e.innerHTML = _("statistic memory left");

	e = document.getElementById("statisticWANLAN");
	e.innerHTML = _("statistic wanlan");
	e = document.getElementById("statisticWANRxPkt");
	e.innerHTML = _("statistic wan rx pkt");
	e = document.getElementById("statisticWANRxBytes");
	e.innerHTML = _("statistic wan rx bytes");
	e = document.getElementById("statisticWANTxPkt");
	e.innerHTML = _("statistic wan tx pkt");
	e = document.getElementById("statisticWANTxBytes");
	e.innerHTML = _("statistic wan tx bytes");
	e = document.getElementById("statisticLANRxPkt");
	e.innerHTML = _("statistic lan rx pkt");
	e = document.getElementById("statisticLANRxBytes");
	e.innerHTML = _("statistic lan rx bytes");
	e = document.getElementById("statisticLANTxPkt");
	e.innerHTML = _("statistic lan tx pkt");
	e = document.getElementById("statisticLANTxBytes");
	e.innerHTML = _("statistic lan tx bytes");

	e = document.getElementById("statisticAllIF");
	e.innerHTML = _("statistic all interface");
}

function PageInit()
{
	initTranslation();
}

function formCheck()
{
	if( document.SystemCommand.command.value == ""){
		alert("Please specify a command.");
		return false;
	}

	return true;
}

</script>

</head>
<body onload="PageInit()">
<table class="body"><tr><td>

<h1 id="statisticTitle">Statistic</h1>
<p id="statisticIntroduction"> Take a look at the Ralink SoC statistics </p>


<table border="1" cellpadding="2" cellspacing="1" width="95%">
<tbody>

<!-- =================  MEMORY  ================= -->
<tr>
  <td class="title" colspan="2" id="statisticMM">Memory</td>
</tr>
<tr>
  <td class="head" id="statisticMMTotal">Memory total: </td>
  <td> <% getMemTotalASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticMMLeft">Memory left: </td>
  <td> <% getMemLeftASP(); %></td>
</tr>


<!-- =================  WAN/LAN  ================== -->
<tr>
  <td class="title" colspan="2" id="statisticWANLAN">WAN/LAN</td>
</tr>
<tr>
  <td class="head" id="statisticWANRxPkt">WAN Rx packets: </td>
  <td> <% getWANRxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANRxBytes">WAN Rx bytes: </td>
  <td> <% getWANRxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANTxPkt">WAN Tx packets: </td>
  <td> <% getWANTxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticWANTxBytes">WAN Tx bytes: </td>
  <td> <% getWANTxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANRxPkt">LAN Rx packets: &nbsp; &nbsp; &nbsp; &nbsp;</td>
  <td> <% getLANRxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANRxBytes">LAN Rx bytes: </td>
  <td> <% getLANRxByteASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANTxPkt">LAN Tx packets: </td>
  <td> <% getLANTxPacketASP(); %></td>
</tr>
<tr>
  <td class="head" id="statisticLANTxBytes">LAN Tx bytes: </td>
  <td> <% getLANTxByteASP(); %></td>
</tr>

<!-- =================  ALL  ================= -->
<tr>
  <td class="title" colspan="2" id="statisticAllIF">All interfaces</td>
<tr>

<script type="text/javascript">
var i;
var a = new Array();
a = [<% getAllNICStatisticASP(); %>];
for(i=0; i<a.length; i+=5){
	// name
	document.write("<tr> <td class=head> Name </td><td class=head>");
	document.write(a[i]);
	document.write("</td></tr>");

	// Order is important! rxpacket->rxbyte->txpacket->txbyte
	// rxpacket
	document.write("<tr> <td class=head> Rx Packet </td><td>");
	document.write(a[i+1]);
	document.write("</td></tr>");

	// rxbyte
	document.write("<tr> <td class=head> Rx Byte </td><td>");
	document.write(a[i+2]);
	document.write("</td></tr>");

	// txpacket
	document.write("<tr> <td class=head> Tx Packet </td><td>");
	document.write(a[i+3]);
	document.write("</td></tr>");

	// txbyte
	document.write("<tr> <td class=head> Tx Byte </td><td>");
	document.write(a[i+4]);
	document.write("</td></tr>");
}
</script>

</tbody>
</table>

</td></tr></table>
</body></html>

