<!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="refresh" CONTENT="3; URL=./apstatistics.asp">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<title>Ralink Wireless AP Statistics</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
}

function PageInit()
{
	initTranslation();
}
</script>
</head>


<body onload="PageInit()">
<table class="body"><tr><td>

<h1 id="apStatisticTitle">AP Wireless Statistics</h1>
<p id="apStatisticIntroduction">Wireless TX and RX Statistics</p>
<hr />

<table width="540" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td class="title" colspan="2" id="statisticTx">Transmit Statistics</td>
  </tr>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">Tx Success</td>
    <td><% getRaixApStats("TxSucc"); %></td>
  </tr>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">Tx Retry Count</td>
    <td><% getRaixApStats("TxRetry"); %></td>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">Tx Fail after retry</td>
    <td><% getRaixApStats("TxFail"); %></td>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">RTS Sucessfully Receive CTS</td>
    <td><% getRaixApStats("RTSSucc"); %></td>
  </tr>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">RTS Fail To Receive CTS</td>
    <td><% getRaixApStats("RTSFail"); %></td>
  </tr>
</table>

<table width="540" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td class="title" colspan="2" id="statisticRx">Receive Statistics</td>
  </tr>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">Frames Received Successfully</td>
    <td><% getRaixApStats("RxSucc"); %></td>
  </tr>
  <tr>
    <td width="65%" bgcolor="#E8F8FF">Frames Received With CRC Error</td>
    <td><% getRaixApStats("FCSError"); %></td>
  </tr>
</table>

<table width="540" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td class="title" colspan="2">SNR</td>
  </tr>
  <tr>
    <td class="head">SNR</td>
    <td><% getRaixApSNR(0); %>, <% getApSNR(1); %>, <% getApSNR(2); %></td>
  </tr>
</table>

</form>
<form method=post name="ap_statistics" action="/goform/resetApCounters">
<table width="540" border="0" cellpadding="2" cellspacing="1">
  <tr align="center">
    <td>	
      <input type="submit" style="{width:120px;}" value="Reset Counters" id="apStatResetCounter">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body>
</html>


