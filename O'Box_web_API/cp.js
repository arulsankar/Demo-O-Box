var pollhttp;
var upgradehttp;
var scenehttp;
var topohttp;
var stathttp;
var atsthttp;
var racphttp;
var polltmr = null;
var pollwait = null;
var divcur = new Array();
var divcon = new Array();
var divinfo = new Array();
var nodes = new Array();
var switch_binary_nodes = new Array();
var nodename = new Array();
var nodeloc = new Array();
var nodegrp = new Array();
var nodegrpgrp = new Array();
var nodepoll = new Array();
var nodepollpoll = new Array();
var astate = false;
var needsave = 0;
var nodecount;
var nodeid;
var sucnodeid;
var tt_top = 3;
var tt_left = 3;
var tt_maxw = 300;
var tt_speed = 10;
var tt_timer = 20;
var tt_endalpha = 95;
var tt_alpha = 0;
var tt_h = 0;
var tt = document.createElement('div');
var t = document.createElement('div');
var c = document.createElement('div');
var b = document.createElement('div');
var ie = document.all ? true : false;
var curnode = null;
var curscene = null;
var scenes = new Array();
var routes = new Array();
var curclassstat = null;
var classstats = new Array();
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    pollhttp = new XMLHttpRequest();
} else {
    pollhttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    upgradehttp = new XMLHttpRequest();
} else {
    upgradehttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    scenehttp = new XMLHttpRequest();
} else {
    scenehttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    topohttp = new XMLHttpRequest();
} else {
    topohttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    stathttp = new XMLHttpRequest();
} else {
    stathttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    atsthttp = new XMLHttpRequest();
} else {
    atsthttp = new ActiveXObject("Microsoft.XMLHTTP");
}
if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
    racphttp = new XMLHttpRequest();
} else {
    racphttp = new ActiveXObject("Microsoft.XMLHTTP");
}

function OptionGroup(label, disabled) {
    var element = document.createElement('optgroup');
    if (disabled !== undefined) element.disabled = disabled;
    if (label !== undefined) element.label = label;
    return element;
}

function SaveNode(newid) {
    var i = newid.substr(4);
    var c = -1;
    if (curnode != null)
        c = curnode.substr(4);
    //document.getElementById('divconfigcur').innerHTML = divcur[i];
    //document.getElementById('divconfigcon').innerHTML = divcon[i];
    //document.getElementById('divconfiginfo').innerHTML = divinfo[i];
    document.getElementById('controls').style.display = 'none';
    if (c != -1) {
        if (i != c)
            document.getElementById(curnode).className = 'normal';
    } else {
        document.getElementById('configcur').disabled = false;
        document.getElementById('configcon').disabled = false;
        document.getElementById('configinfo').disabled = false;
    }
    curnode = newid;
    DoNodeHelp();
    UpdateSceneValues(i);
    $('#devices tr.success').removeClass('success');
    $('#' + curnode).addClass('success');
    return true;
}

function ClearNode() {
    if (curnode != null) {
        document.getElementById(curnode).className = 'normal';
        document.NodePost.nodeops.selectedIndex = 0;
        document.getElementById('divconfigcur').innerHTML = '';
        document.getElementById('divconfigcon').innerHTML = '';
        document.getElementById('divconfiginfo').innerHTML = '';
        document.getElementById('nodeinfo').style.display = 'none';
        document.getElementById('nodecntl').style.display = 'none';
        UpdateSceneValues(-1);
        curnode = null;
    }
    return true;
}

function DisplayNode(n) {
    return true;
}

function PollTimeout() {
    pollhttp.abort();
    Poll();
}

function DoUpgrade() 
{
        upgradehttp.open("GET", 'upgrade.xml', true);
        upgradehttp.onreadystatechange = UpgradeReply;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            upgradehttp.send(null);
        } else { // code for IE6, IE5
            upgradehttp.send();
        }
}

function UpgradeReply()
{
var xml;
if (upgradehttp.readyState == 4 && upgradehttp.status == 200)
{
 console.log(upgradehttp);
}
}

function Poll() {
    try {
        pollhttp.open("GET", 'devicepollingevery3seconds', true);
        pollhttp.onreadystatechange = PollReply;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            pollhttp.send(null);
        } else { // code for IE6, IE5
            pollhttp.send();
        }
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    } catch (e) {
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    }
}


function PollReply() {
    var xml;
    if (pollhttp.readyState == 4 && pollhttp.status == 200) {
        clearTimeout(pollwait);
        xml = pollhttp.responseXML;
        var poll_elems = xml.getElementsByTagName('poll');
        if (poll_elems.length > 0) {
            var changed = false;
            var poll_elem = poll_elems[0];
            if (poll_elem.getAttribute('homeid') != document.getElementById('homeid').value)
                document.getElementById('homeid').value = poll_elem.getAttribute('homeid');
            if (poll_elem.getAttribute('nodecount') != nodecount) {
                nodecount = poll_elem.getAttribute('nodecount');
                document.getElementById('nodecount').value = nodecount;
            }
            if (poll_elem.getAttribute('nodeid') != nodeid) {
                nodeid = poll_elem.getAttribute('nodeid');
            }
            if (poll_elem.getAttribute('sucnodeid') != sucnodeid) {
                sucnodeid = poll_elem.getAttribute('sucnodeid');
                document.getElementById('sucnodeid').value = sucnodeid;
            }
            if (poll_elem.getAttribute('cmode') != document.getElementById('cmode').value)
                document.getElementById('cmode').value = poll_elem.getAttribute('cmode');
            if (poll_elem.getAttribute('save') != needsave) {
                needsave = poll_elem.getAttribute('save');
                if (needsave == '1') {
                    document.getElementById('saveinfo').style.display = 'block';
                } else {
                    document.getElementById('saveinfo').style.display = 'none';
                }
            }
            if (poll_elem.getAttribute('noop') == '1') {
                var testhealreport = document.getElementById('testhealreport');
                testhealreport.innerHTML = testhealreport.innerHTML + 'No Operation message completed.<br>';
            }
            var admin_elem = xml.getElementsByTagName('admin');
            var admin_elem = admin_elem[0];
            if (admin_elem.getAttribute('active') == 'true') {
            //ainfo = document.getElementById('adminfo');
	    //acntl.innerHTML = '';
	    //document.AdmPost.admgo.style.display = 'none';
	    //   ainfo.innerHTML = '';
        	//ainfo.style.display = 'block';
                //$('.rsb').css('display','');
		$('.alert-box').css("display",'');
                if (!astate) {
                    document.AdmPost.admgo.style.display = 'none';
                    document.AdmPost.admcan.style.display = 'none';
                    document.AdmPost.adminops.disabled = true;
                    astate = true;
                }
            } else if (admin_elem.getAttribute('active') == 'false') {
		$('.alert-box').fadeOut(2500);
                if (astate) {
                    document.AdmPost.admgo.style.display = 'none';
                    document.AdmPost.admcan.style.display = 'none';
                    document.AdmPost.adminops.disabled = false;
                    astate = false;
                }
            }
            if (admin_elem.firstChild != null) {
                ainfo = document.getElementById('adminfo');
                ainfo.innerHTML = admin_elem.firstChild.nodeValue;
                ainfo.style.display = 'block';
            }
            var update_elem = xml.getElementsByTagName('update');
            if (update_elem.length > 0) {
                var remove = update_elem[0].getAttribute('remove');
                if (remove != undefined) {
                    var remnodes = remove.split(',');
                    changed = true;
                    for (var i = 0; i < remnodes.length; i++) {
                        nodes[remnodes[i]] = null;
                        if (curnode == ('node' + remnodes[i]))
                            ClearNode();
                    }
                }
            }
            var node_elems = xml.getElementsByTagName('node');
            changed |= node_elems.length > 0;
            for (var i = 0; i < node_elems.length; i++) {
                var node_elem = node_elems[i];
                var id = node_elem.getAttribute('id');
                nodes[id] = {
                    time: node_elem.getAttribute('time'),
                    btype: node_elem.getAttribute('btype'),
                    id: node_elem.getAttribute('id'),
                    gtype: node_elem.getAttribute('gtype'),
                    manufacturer: node_elem.getAttribute('manufacturer'),
                    product: node_elem.getAttribute('product'),
                    name: node_elem.getAttribute('name'),
                    location: node_elem.getAttribute('location'),
                    listening: node_elem.getAttribute('listening') == 'true',
                    frequent: node_elem.getAttribute('frequent') == 'true',
                    zwaveplus: node_elem.getAttribute('zwaveplus') == 'true',
                    beam: node_elem.getAttribute('beam') == 'true',
                    routing: node_elem.getAttribute('routing') == 'true',
                    security: node_elem.getAttribute('security') == 'true',
                    status: node_elem.getAttribute('status'),
                    values: null,
                    groups: null
                };
                var k = 0;
                var values_node = node_elem.getElementsByTagName('value');
                var id_node = nodes[id];
                id_node.values = new Array();
                for (var j = 0; j < values_node.length; j++) {
                    var  values = values_node[j];
                    id_node.values[k] = {
                        readonly: values.getAttribute('readonly') == 'true',
                        genre:  values.getAttribute('genre'),
                        cclass:  values.getAttribute('class'),
                        type:  values.getAttribute('type'),
                        instance:  values.getAttribute('instance'),
                        index:  values.getAttribute('index'),
                        label:  values.getAttribute('label'),
                        units:  values.getAttribute('units'),
                        polled:  values.getAttribute('polled') == true,
                        help: null,
                        value: null
                    };
                    var help =  values.getElementsByTagName('help');
                    var node_values = id_node.values[k];
                    if (help.length > 0)
                        node_values.help = help[0].firstChild.nodeValue;
                    else
                        node_values.help = '';
                    if (node_values.type == 'list') {
                        var items =  values.getElementsByTagName('item');
                        var current =  values.getAttribute('current');
                        node_values.value = new Array();
                        for (var l = 0; l < items.length; l++) {
                            node_values.value[l] = {
                                item: items[l].firstChild.nodeValue,
                                selected: (current == items[l].firstChild.nodeValue)
                            };
                        }
                    } else if ( values.firstChild != null)
                        node_values.value =  values.firstChild.nodeValue;
                    else
                        node_values.value = '0';
                    k++;
                }
                var groups = node_elem.getElementsByTagName('groups');
                id_node.groups = new Array();
                groups = groups[0].getElementsByTagName('group');
                k = 0;
                for (var j = 0; j < groups.length; j++) {
                    var group = groups[j];
                    id_node.groups[k] = {
                        id: group.getAttribute('ind'),
                        max: group.getAttribute('max'),
                        label: group.getAttribute('label'),
                        nodes: null
                    };
                    if (group.firstChild != null)
                        id_node.groups[k].nodes = group.firstChild.nodeValue.split(',');
                    else
                        id_node.groups[k].nodes = new Array();
                    k++;
                }
            }
            var log_elems = xml.getElementsByTagName('log');
            var log_elem = log_elems[0];
            if (log_elems != null && log_elem.getAttribute('size') > 0) {
                var ta = document.getElementById('logdata');
                ta.innerHTML = ta.innerHTML + return2br(log_elem.firstChild.nodeValue);
                ta.scrollTop = ta.scrollHeight;
            }
            if (changed) {
                var stuff = '';
                var serial_no = 0;
                for (var i = 1; i < nodes.length; i++) {
                    var node = nodes[i];
                    if (node == null)
                        continue;
                    if (node != null)
                       serial_no += 1
                    var dt = new Date(node.time * 1000);
                    var yd = new Date(dt.getDate() - 1);
                    var ts;
                    var ext;
                    var exthelp;
                    if (dt < yd)
                        ts = moment(dt).format('LT')  //dt.toLocaleDateString() + ' ' + dt.toLocaleTimeString();
                    else
                        ts = moment(dt).format('LT') //dt.toLocaleTimeString(navigator.language, {hour:'2-digit',minute:'2-digit'}); //
                    var val = '';
                    //alert(node.values.toSource());
                    if (node.values.length > 0)
                        for (var j = 0; j < node.values.length; j++) {
			     //  alert(j);
			    // alert(node.values[j].type);
                            if (node.values[j].genre != 'user')
                                continue;
                            if (node.values[j].type == 'list') {
                                for (var l = 0; l < node.values[j].value.length; l++)
                                    if (!node.values[j].value[l].selected)
                                        continue;
                                    else
                                        val = node.values[j].value[l].item;
                            } else if (node.values[j] != null) {
                                val = node.values[j].value;
				//alert(node.values[j].cclass);
				//for (var z = 0; z < 6; z++)
				//{//alert(node.values[6].cclass);
                                //alert(node.product.indexOf("Door"));
                                if ((node.product.indexOf("Door") != -1) || (node.product.indexOf("Door/Window") != -1))
                                {
                                    if (val == 'False')
                                    {
                                        val = 'Closed';
                                        val = '<span style="color:green;">' + val + '</span>';
                                    }
                                    else if (val == 'True')
                                    {
                                        val = 'Opened';
                                        val = '<span style="color:red;">' + val + '</span>';
                                    }
                                }
                                if ((node.product.indexOf("Lock") != -1) || (node.product.indexOf("Deadbolt") != -1))
                                {
                                    if (val == 'True')
                                    {
                                        val = 'Closed';
                                        val = '<span style="color:green;">' + val + '</span>';
                                    }
                                    else if (val == 'False')
                                    {
                                        val = 'Opened';
                                        val = '<span style="color:red;">' + val + '</span>';
                                    }
                                }
				//alert(id_node.gtype);
				if (node.gtype == "Routing Alarm Sensor")
				{
				for(var z = 0; z < node.values.length; z++)
				    {
				    if(node.values[z].cclass == "SENSOR ALARM")
                                      {
                                      if(node.values[z].label == "Flood")
                                       {
                                        if(node.values[z].value == "255")
				     	 {
                                       	 val = 'Detected';
                                       	 val = '<span style="color:red;">' + val + '</span>';
                                      	 }
                                    	 else
                                      	 {
                                         val = 'Clear';
                                         val = '<span style="color:green;">' + val + '</span>';
                                         }
                                        }
                                       }
				     }
				}
				if (node.gtype == "Home Security Sensor")
				{
				for(var y = 0; y < node.values.length; y++)
				    {
				    if(node.values[y].cclass == "ALARM")
                                      {
                                      if(node.values[y].label =="Burglar")
                                       {
                                        if(node.values[y].value == "8")
				     	 {//alert(id_node.gtype);
                                       	 val = 'Detected';
                                       	 val = '<span style="color:red;">' + val + '</span>';
                                      	 }
					else if(node.values[y].value == "3")
						{break;}
                                    	 else
                                      	 {
                                         val = 'Clear';
                                         val = '<span style="color:green;">' + val + '</span>';
                                         }
                                        }
                                       }
				     }
				}
                                if (node.values[j].cclass == "SENSOR BINARY")
                                {
                                    if (val == 'False')
                                    {
                                        val = 'Clear';
                                        val = '<span style="color:green;">' + val + '</span>';
                                    }
                                    else if (val == 'True')
                                    {
                                        val = 'Detected';
                                        val = '<span style="color:red;">' + val + '</span>';
                                    }
                                }
                                    if (val == 'False')
                                    {
                                        val = 'Clear';
                                        val = '<span style="color:green;">' + val + '</span>';
                                    }
                                    else if (val == 'True')
                                    {
                                        val = 'Detected';
                                        val = '<span style="color:red;">' + val + '</span>';
                                    }
                                //if(node.values[2].cclass == "ALARM" || node.values[3].cclass == "ALARM")
                                //{
                                    //if(node.values[2].label == "Alarm Type" || node.values[3].label == "Alarm Level")
                                    //{
				      //if(node.values[2].value == "0" && node.values[3].value == "0")
                                      //{
                                        //break;
                                      //}
				      //if(node.values[2].value == "18" || (node.values[2].value == "24" && node.values[3].value == "1"))
					//{                                       
					//val = 'Detected';
                                        //val = '<span style="color:red;">' + val + '</span>';
					//}
                                      //else
					//{                                       
					//val = 'Clear';
                                        //val = '<span style="color:red;">' + val + '</span>';
					//}
                                    //}
                                //}
			      //}
                            }
                            break;
                        }
                    ext = ' ';
                    exthelp = '';
                    if (node.id == nodeid) {
                        ext += '*';
                        exthelp += 'controller, ';
                    }
                    if (node.listening) {
                        ext += 'L';
                        exthelp += 'listening, ';
                    }
                    if (node.frequent) {
                        ext += 'F';
                        exthelp += 'FLiRS, ';
                    }
                    if (node.beam) {
                        ext += 'B';
                        exthelp += 'beaming, ';
                    }
                    if (node.routing) {
                        ext += 'R';
                        exthelp += 'routing, ';
                    }
                    if (node.security) {
                        ext += 'S';
                        exthelp += 'security, ';
                    }
                    if (node.zwaveplus) {
                        ext += "+";
                        exthelp += 'ZwavePlus, ';
                    }
                    if (exthelp.length > 0)
                        exthelp = exthelp.substr(0, exthelp.length - 2);
                    var keyword = 'refreshnode';
                    if (i!=1)
                    {    
                      if (node.manufacturer != '' && node.product != ''){               
                      //document.getElementById('nick_name_input').innerHTML = '<input type="text" size="1" class="legend form-control" id="nick_name' + i + '">'
                      stuff += '<tr id="node' + i + '" onmouseover="return SaveNode(this.id);"><td>' + (serial_no-1)  + '</td><td>'  + '<span>' + node.manufacturer + ' ' + node.product  + '</span>' + ' ' + '<a class="btn glyphi_icon" role="button" id="DeviceValueListModal" onclick="return open_value_modal('+"'"+ i +"'" + ');">' + '<em class="glyphicon glyphicon-eye-open"></em>' + '</a>' +  '</td><td>' + '<span>'  + node.name + '</span>' + '  ' + '<a class="btn glyphi_icon" role="button" id="modal-154656" onclick="return open_modal('+"'"+ node.name+"'" + ');">' + '<em class="glyphicon glyphicon-pencil"></em>' + '</a>' + '</td><td>' + val + '</td><td>' + ts + '</td><td>' + node.status + '</td><td>' + '<button class="btn btn-lg btn-danger btn-circle btn-table btn-add" onclick="return call_add_device(' + "'remd'" + ');">' + '<i class="fa fa-times"></i>' + '</button>'  + '</td><td>' + '<button class="btn btn-lg btn-warning btn-circle btn-table btn-add" onclick="return call_add_device(' + "'refreshnode'" + ');">' + '<i class="fa fa-refresh"></i>' + '</button>' + '</td></tr>';
                    }}
                    $('.alert-box').fadeOut(2500);
		    $('button').prop('disabled',false);
		    $('input.val_chckbox').prop("disabled",false);
 		    $('.glyphi_icon').removeClass('disabled');
                    clearInterval(add_timer);
                    $('#timer_msg').fadeOut(2500);
                    //document.getElementById('timer_msg').style.display = 'none';
                    document.getElementById('controls').style.display = 'none';
                    CreateDivs('user', divcur, i);
                    CreateDivs('config', divcon, i);
                    CreateDivs('system', divinfo, i); 
                    CreateName(node.name, i);
                    CreateLocation(node.location, i);
                    CreateGroup(i);
                    CreatePoll(i);
                    if (node.values[0].cclass == "SWITCH BINARY")
			        {	
			         switch_binary_nodes.push(i);
			        }       
                }
                document.getElementById('tbody').innerHTML = stuff;
                for(var cksb = 0;cksb < switch_binary_nodes.length; cksb++) 
                    {
                         $('#node'+switch_binary_nodes[cksb]+'> td:eq(3)').html($(divcur[switch_binary_nodes[cksb]]).find("tbody > tr:eq(0) > td:eq(1)").html());
	            }
                //$("[name='toggle_select']").bootstrapSwitch();
                //alert(stuff);
                if (curnode != null)
                    SaveNode(curnode);
            }
        if(document.getElementById('tbody').innerHTML == '')
        {
          
          stuff += '<tr><td colspan="8">' + '<kbd>' + '<i class="fa fa-exclamation-triangle"></i>' + ' No devices found. Please click plus button to add the device</kbd>' + '</tr></td>'             
          document.getElementById('tbody').innerHTML = stuff;
          document.getElementById('controls').style.display = 'none';
        }
     
        }
        polltmr = setTimeout(Poll, 750);
    }
}

function BED() {
    var forms = document.forms;
    var off = false;
    (document.DevPost.devname.value.length == 0) && !document.DevPost.usbb.checked;
    var info;
    tt.setAttribute('id', 'tt');
    t.setAttribute('id', 'tttop');
    c.setAttribute('id', 'ttcont');
    b.setAttribute('id', 'ttbot');
    tt.appendChild(t);
    tt.appendChild(c);
    tt.appendChild(b);
    document.body.appendChild(tt);
    tt.style.opacity = 0;
    tt.style.filter = 'alpha(opacity=0)';
    tt.style.display = 'none';
    for (var i = 0; i < forms.length; i++) {
        var form = forms[i];
        if (form.name == '') 
            continue;
        for (var j = 0; j < form.elements.length; j++) {
            var element = form.elements[j];
            if ((element.name == 'initialize') ||
                (element.name == 'devname') ||
                (element.name == 'usbb'))
                continue;
            if ((element.tagName == 'BUTTON') ||
                (element.tagName == 'SELECT') ||
                (element.tagName == 'INPUT'))
                element.disabled = off;
            else
                element.disabled = !off;
        }
    }
    document.getElementById('configcur').disabled = off;
    document.getElementById('configcur').checked = true;
    document.getElementById('configcon').disabled = off;
    document.getElementById('configcon').checked = false;
    document.getElementById('configinfo').disabled = off;
    document.getElementById('configinfo').checked = false;
    // document.NetPost.netops.selectedIndex = 0;
    // document.NetPost.netops.disabled = off;
    info = document.getElementById('netinfo');
    info.style.display = 'none';
    document.AdmPost.adminops.selectedIndex = 0;
    document.AdmPost.adminops.disabled = off;
    info = document.getElementById('adminfo');
    info.style.display = 'none';
    info = document.getElementById('admcntl');
    info.style.display = 'none';
    document.NodePost.nodeops.selectedIndex = 0;
    document.NodePost.nodeops.disabled = off;
    info = document.getElementById('nodeinfo');
    info.style.display = 'none';
    info = document.getElementById('nodecntl');
    info.style.display = 'none';
    if (off) {
        document.getElementById('homeid').value = '';
        document.getElementById('cmode').value = '';
        document.getElementById('nodecount').value = '';
        document.getElementById('sucnodeid').value = '';
        document.getElementById('saveinfo').style.display = 'none';
        document.getElementById('tbody').innerHTML = '';
        document.getElementById('divconfigcur').innerHTML = '';
        document.getElementById('divconfigcon').innerHTML = '';
        document.getElementById('divconfiginfo').innerHTML = '';
        document.getElementById('logdata').innerHTML = '';
    }
    if (!off) {
        Poll();
    } else {
        pollhttp.abort();
        clearTimeout(polltmr);
        clearTimeout(pollwait);
    }
    curnode = null;
}

function ShowToolTip(help, width) {
    tt.style.display = 'block';
    c.innerHTML = help;
    tt.style.width = width ? width + 'px' : 'auto';
    if (!width && ie) {
        t.style.display = 'none';
        b.style.display = 'none';
        tt.style.width = tt.offsetWidth;
        t.style.display = 'block';
        b.style.display = 'block';
    }
    if (tt.offsetWidth > tt_maxw) {
        tt.style.width = tt_maxw + 'px';
    }
    tt_h = parseInt(tt.offsetHeight) + tt_top;
    clearInterval(tt.timer);
    tt.timer = setInterval(function() {
        FadeToolTip(1);
    }, tt_timer)
}

function PosToolTip(e) {
    tt.style.top = ((ie ? event.clientY + document.documentElement.scrollTop : e.pageY) - tt_h) + 'px';
    tt.style.left = ((ie ? event.clientX + document.documentElement.scrollLeft : e.pageX) + tt_left) + 'px';
}

function FadeToolTip(d) {
    var a = tt_alpha;
    if ((a != tt_endalpha && d == 1) || (a != 0 && d == -1)) {
        var i = tt_speed;
        if (tt_endalpha - a < tt_speed && d == 1) {
            i = tt_endalpha - a;
        } else if (tt_alpha < tt_speed && d == -1) {
            i = a;
        }
        tt_alpha = a + (i * d);
        tt.style.opacity = tt_alpha * .01;
        tt.style.filter = 'alpha(opacity=' + tt_alpha + ')';
    } else {
        clearInterval(tt.timer);
        if (d == -1) {
            tt.style.display = 'none';
        }
    }
}

function HideToolTip() {
    clearInterval(tt.timer);
    tt.timer = setInterval(function() {
        FadeToolTip(-1);
    }, tt_timer);
}

function DoConfig(id) {
    if (curnode != null) {
        var dcur = document.getElementById('divconfigcur');
        var dcon = document.getElementById('divconfigcon');
        var dinfo = document.getElementById('divconfiginfo');
        var node = curnode.substr(4);

        dcur.innerHTML = divcur[node];
        dcon.innerHTML = divcon[node];
        dinfo.innerHTML = divinfo[node];
        if (id == 'configcur') {
            dcur.className = '';
            dcon.className = 'hide';
            dinfo.className = 'hide';
        } else if (id == 'configcon') {
            dcon.className = '';
            dcur.className = 'hide';
            dinfo.className = 'hide';
        } else {
            dinfo.className = '';
            dcur.className = 'hide';
            dcon.className = 'hide';
        }
        return true;
    } else {
        return false;
    }
}

function DoValue(id, convert) {
    if (curnode != null) {
        var posthttp;
        var params;
        var node_id = id.split('-')[0]
        var arg = document.getElementById(id).value;
        if (typeof convert == 'undefined') {
            convert = true;
        }
        if (convert) {
            if (arg.toLowerCase() == 'off')
            {
                arg = 'false';
                $('#'+id).val('on');
            }
            else if (arg.toLowerCase() == 'on')
            {
                arg = 'true';
                $('#'+id).val('off');
            }
        }
        params = id + '=' + arg;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            posthttp = new XMLHttpRequest();
        } else {
            posthttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        posthttp.open('POST', 'valuepost.html', true);
        posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        posthttp.send(params);
        RequestAll(node_id);
    }
    return false;
}

function DoButton(id, pushed) {
    if (curnode != null) {
        var posthttp;
        var params;
        var arg = document.getElementById(id).value;

        if (pushed)
            arg = 'true';
        else
            arg = 'false';
        params = id + '=' + arg;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            posthttp = new XMLHttpRequest();
        } else {
            posthttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        posthttp.open('POST', 'buttonpost.html', true);
        posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        posthttp.send(params);
    }
    return false;
}

function DoDevUSB() {
    document.DevPost.devname.disabled = document.DevPost.usbb.checked;
    return true;
}

function DoDevPost(fun) {
    if (document.DevPost.devname.value.length > 0 || document.DevPost.usbb.checked) {
        var posthttp;
        var params;
        params = 'dev=' + document.DevPost.devname.value + '&fn=' + fun + '&usb=' + document.DevPost.usbb.checked;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            posthttp = new XMLHttpRequest();
        } else {
            posthttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        posthttp.open('POST', 'devpost.html', true);
        posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        posthttp.send(params);
        if (fun == 'close') {
            document.DevPost.devname = '';
            document.DevPost.usbb.checked = false;
        }
        BED();
    }
    return false;
}

function DoNetHelp() {
    var ninfo = document.getElementById('netinfo');
    var scencntl = document.getElementById('scencntl');
    var topocntl = document.getElementById('topocntl');
    var topo = document.getElementById('topo');
    var statcntl = document.getElementById('statcntl');
    var statnet = document.getElementById('statnet');
    var statnode = document.getElementById('statnode');
    var statclass = document.getElementById('statclass');
    var thcntl = document.getElementById('thcntl');
    var testhealreport = document.getElementById('testhealreport');
    if (document.NetPost.netops.value == 'scen') {
        ninfo.innerHTML = 'Scene management and execution.';
        ninfo.style.display = 'block';
        scencntl.style.display = 'block';
        topocntl.style.display = 'none';
        topo.style.display = 'none';
        statcntl.style.display = 'none';
        statnet.style.display = 'none';
        statnode.style.display = 'none';
        statclass.style.display = 'none';
        thcntl.style.display = 'none';
        testhealreport.style.display = 'none';
        SceneLoad('load');
    } else if (document.NetPost.netops.value == 'topo') {
        ninfo.innerHTML = 'Topology views';
        ninfo.style.display = 'block';
        scencntl.style.display = 'none';
        topocntl.style.display = 'block';
        topo.style.display = 'block';
        statcntl.style.display = 'none';
        statnet.style.display = 'none';
        statnode.style.display = 'none';
        statclass.style.display = 'none';
        thcntl.style.display = 'none';
        testhealreport.style.display = 'none';
        curscene = null;
        TopoLoad('load');
    } else if (document.NetPost.netops.value == 'stat') {
        ninfo.innerHTML = 'Statistic views';
        ninfo.style.display = 'block';
        scencntl.style.display = 'none';
        topocntl.style.display = 'none';
        topo.style.display = 'none';
        statcntl.style.display = 'block';
        statnet.style.display = 'block';
        statnode.style.display = 'block';
        thcntl.style.display = 'none';
        testhealreport.style.display = 'none';
        curscene = null;
        StatLoad('load');
    } else if (document.NetPost.netops.value == 'test') {
        ninfo.innerHTML = 'Test & Heal Network';
        ninfo.style.display = 'block';
        scencntl.style.display = 'none';
        topocntl.style.display = 'none';
        topo.style.display = 'none';
        statcntl.style.display = 'none';
        statnet.style.display = 'none';
        statnode.style.display = 'none';
        statclass.style.display = 'none';
        thcntl.style.display = 'block';
        testhealreport.style.display = 'block';
        curscene = null;
    } else {
        ninfo.style.display = 'none';
        scencntl.style.display = 'none';
        topocntl.style.display = 'none';
        topo.style.display = 'none';
        statcntl.style.display = 'none';
        statnet.style.display = 'none';
        statnode.style.display = 'none';
        statclass.style.display = 'none';
        thcntl.style.display = 'none';
        testhealreport.style.display = 'none';
        curscene = null;
    }
    return true;
}


function DoAdmPost(can) {
    var posthttp;
    var fun;
    var params;

    if (can) {
        fun = 'cancel';
        ainfo = document.getElementById('adminfo');
        ainfo.innerHTML = 'Cancelling controller function.';
        ainfo.style.display = 'block';
        $('#timer_msg').fadeOut(2500);
        //$('#timer_msg').css('display','none');
        $('.alert-box').fadeOut(2500);
        $('button').prop('disabled',false);
        $('input.val_chckbox').prop("disabled",false);
 	$('.glyphi_icon').removeClass('disabled');
        clearInterval(add_timer);
    } else {
        fun = document.AdmPost.adminops.value;
        if (fun == 'choice')
            return false;
    }
    params = 'fun=' + fun;

    if (fun == 'hnf' || fun == 'remfn' || fun == 'repfn' || fun == 'reqnu' ||
        fun == 'reqnnu' || fun == 'assrr' || fun == 'delarr' || fun == 'reps' ||
        fun == 'addbtn' || fun == 'delbtn' || fun == 'refreshnode') {
        if (curnode == null) {
            ainfo = document.getElementById('adminfo');
            ainfo.innerHTML = 'Must select a node below for this function.';
            ainfo.style.display = 'block';
            return false;
        }
        params += '&node=' + curnode;
    } else if (fun == 'snif') {
        if (curnode == null)
            params += '&node=node255';
        else
            params += '&node=' + curnode;
    }

    if (fun == 'addbtn' || fun == 'delbtn') {
        if (document.AdmPost.button.value.length == 0) {
            ainfo = document.getElementById('adminfo');
            ainfo.innerHTML = 'Button number is required.';
            ainfo.style.display = 'block';
            document.AdmPost.button.select();
            return false;
        }
        params += '&button=' + document.AdmPost.button.value;
    }

    if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
        posthttp = new XMLHttpRequest();
    } else {
        posthttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    posthttp.open('POST', 'admpost.html', true);
    posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    posthttp.send(params);
    if (fun == 'remc' || fun == 'remd') {
        document.getElementById('divconfigcur').innerHTML = '';
        document.getElementById('divconfigcon').innerHTML = '';
        document.getElementById('divconfiginfo').innerHTML = '';
        curnode = null;
    }
    return false;
}

function DoAdmHelp() {
    ainfo = document.getElementById('adminfo');
    var acntl = document.getElementById('admcntl');
    acntl.innerHTML = '';
    document.AdmPost.admgo.style.display = 'none';
    if (document.AdmPost.adminops.value == 'addd') {
        ainfo.innerHTML = 'Add a new device or controller to the Z-Wave network.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'addds') {
        ainfo.innerHTML = 'Add Device -> Tap the Z-Wave button on the device which you want to detect by this O Box';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'cprim') {
        ainfo.innerHTML = 'Create new primary controller in place of dead old controller.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'rconf') {
        ainfo.innerHTML = 'Receive configuration from another controller.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'remd') {
        ainfo.innerHTML = 'Remove Device -> Tap the Z-Wave button on the device which you want to detect by this O Box';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'remfn') {
        ainfo.innerHTML = 'Remove a failed node that is on the controller\'s list of failed nodes.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'hnf') {
        ainfo.innerHTML = 'Check whether a node is in the controller\'s failed nodes list.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'repfn') {
        ainfo.innerHTML = 'Replace a failed device with a working device.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'tranpr') {
        ainfo.innerHTML = 'Transfer primary to a new controller and make current secondary.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'reqnu') {
        ainfo.style.display = 'block';
        ainfo.innerHTML = 'Update the controller with network information from the SUC/SIS.';
    } else if (document.AdmPost.adminops.value == 'reqnnu') {
        ainfo.innerHTML = 'Get a node to rebuild its neighbor list.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'assrr') {
        ainfo.innerHTML = 'Assign a network return route to a device.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'delarr') {
        ainfo.innerHTML = 'Delete all network return routes from a device.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'snif') {
        ainfo.innerHTML = 'Send a node information frame.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'reps') {
        ainfo.innerHTML = 'Send information from primary to secondary.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'addbtn' ||
        document.AdmPost.adminops.value == 'delbtn') {
        if (curnode == null) {
            ainfo.innerHTML = 'Must select a node below for this function.';
            ainfo.style.display = 'block';
            document.AdmPost.adminops.selectedIndex = 0;
            document.AdmPost.admgo.style.display = 'none';
            return false;
        }
        acntl.innerHTML = '<label style="margin-left: 10px;"><span class="legend">Button number:&nbsp;</span></label><input type="text" class="legend form-control" size="3" id="button" value="">';
        acntl.style.display = 'block';
        document.AdmPost.button.select();
        if (document.AdmPost.adminops.value == 'addbtn')
            ainfo.innerHTML = 'Add a button from a handheld.';
        else
            ainfo.innerHTML = 'Remove a button from a handheld.';
        ainfo.style.display = 'block';
    } else if (document.AdmPost.adminops.value == 'refreshnode') {
        ainfo.innerHTML = 'Refresh Node Info';
        ainfo.style.display = 'block';
    } else {
        ainfo.style.display = 'none';
        document.AdmPost.admgo.style.display = 'none';
    }
    return true;
}

function DoNodePost(val) {
    var posthttp;
    var fun;
    var params;

    fun = document.NodePost.nodeops.value;
    if (fun == 'choice')
        return false;

    params = 'fun=' + fun + '&node=' + curnode + '&value=' + val;
    if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
        posthttp = new XMLHttpRequest();
    } else {
        posthttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    posthttp.open('POST', 'nodepost.html', true);
    posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    posthttp.send(params);
    call_add_device('refreshnode');
    return false;
}

function DoNodeHelp() {
    var ninfo = document.getElementById('nodeinfo');
    if (curnode == null) {
        ninfo.innerHTML = 'Must select a node below for this function.';
        ninfo.style.display = 'block';
        document.NodePost.nodeops.selectedIndex = 0;
        return false;
    }
    var node = curnode.substr(4);
    var ncntl = document.getElementById('nodecntl');
    if (document.NodePost.nodeops.value == 'nam') {
        ninfo.innerHTML = 'Naming functions.';
        ninfo.style.display = 'block';
        ncntl.innerHTML = nodename[node];
        ncntl.style.display = 'block';
        document.NodePost.name.select();
    } else if (document.NodePost.nodeops.value == 'loc') {
        ninfo.innerHTML = 'Location functions.';
        ninfo.style.display = 'block';
        ncntl.innerHTML = nodeloc[node];
        ncntl.style.display = 'block';
        document.NodePost.location.select();
    } else if (document.NodePost.nodeops.value == 'grp') {
        ninfo.innerHTML = 'Group/Association functions';
        ninfo.style.display = 'block';
        ncntl.innerHTML = nodegrp[node] + nodegrpgrp[node][1];
        ncntl.style.display = 'block';
    } else if (document.NodePost.nodeops.value == 'pol') {
        ninfo.innerHTML = 'Polling settings';
        ninfo.style.display = 'block';
        ncntl.innerHTML = nodepoll[node];
        ncntl.style.display = 'block';
        DoPoll();
    } else {
        ninfo.style.display = 'none';
        ncntl.style.display = 'none';
    }
    return true;
}

function DoGroup() {
    var node = curnode.substr(4);
    var ngrp = document.getElementById('nodegrp');

    ngrp.innerHTML = nodegrpgrp[node][document.NodePost.group.value];
    return true;
}

function DoGrpPost() {
    var posthttp;
    var params = 'fun=group&node=' + curnode + '&num=' + document.NodePost.group.value + '&groups=';
    var opts = document.NodePost.groups.options;
    var i;

    for (i = 0; i < opts.length; i++)
        if (opts[i].selected) {
            params += opts[i].text + ',';
        }

    if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
        posthttp = new XMLHttpRequest();
    } else {
        posthttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    posthttp.open('POST', 'grouppost.html', true);
    posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    posthttp.send(params);

    return false;
}

function DoPoll() {
    var node = curnode.substr(4);
    var npoll = document.getElementById('nodepoll');
    var polled = document.getElementById('polled');

    if (polled != null)
        npoll.innerHTML = nodepollpoll[node][polled.value];
    return true;
}

function DoPollPost() {
    var posthttp;
    var params = 'fun=poll&node=' + curnode + '&ids=';
    var opts = document.NodePost.polls.options;
    var i;

    for (i = 0; i < opts.length; i++)
        params += opts[i].id + ',';
    params += '&poll=';
    for (i = 0; i < opts.length; i++)
        if (opts[i].selected)
            params += '1,';
        else
            params += '0,';


    if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
        posthttp = new XMLHttpRequest();
    } else {
        posthttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    posthttp.open('POST', 'pollpost.html', true);
    posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    posthttp.send(params);

    return false;
}

function DoSavePost() {
    var posthttp;
    var params = 'fun=save';

    if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
        posthttp = new XMLHttpRequest();
    } else {
        posthttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    posthttp.open('POST', 'savepost.html', true);
    posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    posthttp.send(params);

    return false;
}

function SceneLoad(fun) {
    var params = 'fun=' + fun;
    if (fun == 'load') {
        DisplaySceneSceneValue(null);
        var scenescenevalues = document.getElementById('scenescenevalues');
        while (scenescenevalues.options.length > 0)
            scenescenevalues.remove(0);
    }
    if (fun == 'delete') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        DisplaySceneSceneValue(null);
        params += '&id=' + curscene;
        var slt = document.getElementById('scenelabeltext');
        slt.value = '';
    } else if (fun == 'execute') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        params += '&id=' + curscene;
    } else if (fun == 'values') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        params += '&id=' + curscene;
        var slt = document.getElementById('scenelabeltext');
        slt.value = scenes[curscene].label;
        DisplaySceneSceneValue(null);
    } else if (fun == 'label') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        var slt = document.getElementById('scenelabeltext');
        if (slt.value.length == 0) {
            alert('Missing label text');
            return false;
        }
        params += '&id=' + curscene + '&label=' + slt.value;
        slt.value = '';
    } else if (fun == 'addvalue') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        if (curnode == null) {
            alert('Node not selected');
            return false;
        }
        var values = document.getElementById('scenevalues');
        if (values.options.selectedIndex == -1) {
            alert('Value not selected');
            return false;
        }
        var vals = values.options[values.options.selectedIndex].value.split('-');
        if (vals[3] != 'list' && vals[3] != 'bool') {
            var value = document.getElementById('valuetext');
            if (value.value.length == 0) {
                alert('Data not entered');
                return false;
            }
            params += '&id=' + curscene + '&vid=' + vals[0] + '-' + vals[1] + '-' + vals[2] + '-' + vals[3] + '-' + vals[4] + '-' + vals[5] + '&value=' + value.value;
        } else {
            var value = document.getElementById('valueselect');
            params += '&id=' + curscene + '&vid=' + vals[0] + '-' + vals[1] + '-' + vals[2] + '-' + vals[3] + '-' + vals[4] + '-' + vals[5] + '&value=' + value.options[value.selectedIndex].value;
        }
        DisplaySceneSceneValue(null);
    } else if (fun == 'update') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        if (curnode == null) {
            alert('Node not selected');
            return false;
        }
        var values = document.getElementById('scenescenevalues');
        if (values.options.selectedIndex == -1) {
            alert('Value not selected');
            return false;
        }
        var vals = values.options[values.options.selectedIndex].value.split('-');
        if (vals[3] != 'list' && vals[3] != 'bool') {
            var value = document.getElementById('scenevaluetext');
            if (value.value.length == 0) {
                alert('Data not entered');
                return false;
            }
            params += '&id=' + curscene + '&vid=' + vals[0] + '-' + vals[1] + '-' + vals[2] + '-' + vals[3] + '-' + vals[4] + '-' + vals[5] + '&value=' + value.value;
        } else {
            var value = document.getElementById('valueselect');
            params += '&id=' + curscene + '&vid=' + vals[0] + '-' + vals[1] + '-' + vals[2] + '-' + vals[3] + '-' + vals[4] + '-' + vals[5] + '&value=' + value.options[value.selectedIndex].value;
        }
        DisplaySceneSceneValue(null);
    } else if (fun == 'remove') {
        if (curscene == null) {
            alert("Scene not selected");
            return false;
        }
        var values = document.getElementById('scenescenevalues');
        if (values.options.selectedIndex == -1) {
            alert('Scene value not selected');
            return false;
        }
        var vals = values.options[values.options.selectedIndex].value.split('-');
        params += '&id=' + curscene + '&vid=' + vals[0] + '-' + vals[1] + '-' + vals[2] + '-' + vals[3] + '-' + vals[4] + '-' + vals[5];
        DisplaySceneSceneValue(null);
    }
    scenehttp.open('POST', 'scenepost.html', true);
    scenehttp.onreadystatechange = SceneReply;
    scenehttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	alert(params);
    scenehttp.send(params);

    return false;
}

function SceneReply() {
    var xml;
    var elem;

    if (scenehttp.readyState == 4 && scenehttp.status == 200) {
        xml = scenehttp.responseXML;
        scenes_elem = xml.getElementsByTagName('scenes');
        if (scenes_elem.length > 0) {
            var i;
            var id;
            var sceneids = document.getElementById('sceneids');
            var scenevalues = document.getElementById('scenevalues');
            var scenescenevalues = document.getElementById('scenescenevalues');
            var elem = scenes_elem[0];
            i = elem.getAttribute('sceneid');
            if (i != null) {
                scenes = new Array();
                while (sceneids.options.length > 0)
                    sceneids.remove(0);
            }
            i = elem.getAttribute('scenevalue');
            if (i != null) {
                scenes[curscene].values = new Array();
                while (scenescenevalues.options.length > 0)
                    scenescenevalues.remove(0);
            }
            for (i = 0; i < elem.childNodes.length; i++) {
                var children = elem.childNodes[i];
                if (children.nodeType != 1)
                    continue;
                if (children.tagName == 'sceneid') {
                    id = children.getAttribute('id');
                    label = children.getAttribute('label');
                    scenes[id] = {
                        label: label,
                        values: new Array()
                    };
                    sceneids.add(new Option('[' + id + '] ' + label, id));
                } else if (children.tagName == 'scenevalue') {
                    var value = {
                        home: children.getAttribute('home'),
                        node: children.getAttribute('node'),
                        label: children.getAttribute('label'),
                        units: children.getAttribute('units'),
                        type: children.getAttribute('type'),
                        cclass: children.getAttribute('class'),
                        genre: children.getAttribute('genre'),
                        instance: children.getAttribute('instance'),
                        index: children.getAttribute('index'),
                        value: children.firstChild.nodeValue
                    };
                    id = children.getAttribute('id');
                    var node = nodes[value.node];
                    var val = ['[' + value.node + ']'];
                    if(node)
                        val = val.concat([
                            node.product + ': ' + value.value
                        ]);
                    var vid = [
                        value.node,
                        value.cclass,
                        value.genre,
                        value.type,
                        value.instance,
                        value.index
                    ].join('-');
                    scenescenevalues.add(new Option(val.join(' '), vid));
                    scenes[id].values.push(value);
                }
            }
        }
    }
}

function UpdateSceneValues(c) {
    var sv = document.getElementById('scenevalues');
    var node = nodes[c];
    var optgroups = sv.getElementsByTagName('optgroup');
    while (sv.options.length > 0)
        sv.remove(0);
    for(var i=optgroups.length - 1; i>=0; i--)
        sv.removeChild(optgroups[i]);

    if (c == -1)
        return;
    var optgroup = OptionGroup('[' + node.id + '] ' + ' ' + node.product);
    sv.add(optgroup);
    for (var i = 0; i < node.values.length; i++) {
        var value = node.values[i];
        if (value.genre != 'user')
            continue;
        if (value.readonly)
            continue;
        if (value.type == 'button')
            continue;
        var vid = node.id + '-' + value.cclass + '-user-' + value.type + '-' + value.instance + '-' + value.index;
        var label = '[' + value.instance + '] ' + value.label + ': ' + value.value;
        optgroup.appendChild(new Option(label, vid));
    }
    DisplaySceneValue(null);
}

function DisplaySceneValue(opt) {
    var vt = document.getElementById('valuetext');
    var vs = document.getElementById('valueselect');
    var vu = document.getElementById('valueunits');
    if (opt == null) {
        vt.style.display = 'inline';
        vs.style.display = 'none';
        vt.value = '';
        while (vs.options.length > 0)
            vs.remove(0);
        vu.innerHTML = '';
        return false;
    }
    var vals = opt.value.split('-');
    for (var j = 0; j < nodes[vals[0]].values.length; j++) {
        var value = nodes[vals[0]].values[j];
        if (value.cclass == vals[1] &&
            value.genre == 'user' &&
            value.type == vals[3] &&
            value.instance == vals[4] &&
            value.index == vals[5])
            break;
    }
    if (vals[3] == 'list') {
        vt.style.display = 'none';
        vs.style.display = 'inline';
    } else if (vals[3] == 'bool') {
        if (value.value == 'True') {
            vs.add(new Option('On', 'true', true));
            vs.add(new Option('Off', 'false'));
        } else {
            vs.add(new Option('Off', 'false', true));
            vs.add(new Option('On', 'true'));
        }
        vt.style.display = 'none';
        vs.style.display = 'inline';
    } else {
        vt.value = value.value;
        vt.style.display = 'inline';
        vs.style.display = 'none';
    }
    vu.innerHTML = value.units;
    return false;
}

function DisplaySceneSceneValue(opt) {
    var vt = document.getElementById('scenevaluetext');
    var vs = document.getElementById('scenevalueselect');
    var vu = document.getElementById('scenevalueunits');
    if (opt == null) {
        vt.style.display = 'inline';
        vs.style.display = 'none';
        vt.value = '';
        while (vs.options.length > 0)
            vs.remove(0);
        vu.innerHTML = '';
        return false;
    }
    var vals = opt.value.split('-');
    for (var j = 0; j < scenes[curscene].values.length; j++) {
        var value = scenes[curscene].values[j];
        if (value.cclass == vals[1] &&
            value.genre == 'user' &&
            value.type == vals[3] &&
            value.instance == vals[4] &&
            value.index == vals[5])
            break;
    }
    if (vals[3] == 'list') {
        vt.style.display = 'none';
        vs.style.display = 'inline';
    } else if (vals[3] == 'bool') {
        if (value.value == 'True') {
            vs.add(new Option('On', 'on', true));
            vs.add(new Option('Off', 'off'));
        } else {
            vs.add(new Option('Off', 'off', true));
            vs.add(new Option('On', 'on'));
        }
        vt.style.display = 'none';
        vs.style.display = 'inline';
    } else {
        vt.value = value.value;
        vt.style.display = 'inline';
        vs.style.display = 'none';
    }
    vu.innerHTML = value.units;
    return false;
}

function TopoLoad(fun) {
    var params = 'fun=' + fun;
    topohttp.open('POST', 'topopost.html', true);
    topohttp.onreadystatechange = TopoReply;
    topohttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    topohttp.send(params);

    return false;
}

function TopoReply() {
    var xml;
    if (topohttp.readyState == 4 && topohttp.status == 200) {
        xml = topohttp.responseXML;
        var elems = xml.getElementsByTagName('topo');
        if (elems.length > 0) {
            var i;
            var id;
            var list;
            var elem = elems[0];
            for (i = 0; i < elem.childNodes.length; i++) {
                var child = elem.childNodes[i];
                if (child.nodeType != 1)
                    continue;
                if (child.tagName == 'node') {
                    id = child.getAttribute('id');
                    list = child.firstChild.nodeValue;
                    routes[id] = list.split(',');
                }
            }
            var stuff = '<tr><th>Nodes</th>';
            var topohead = document.getElementById('topohead');
            var node = nodes[i];
            for (i = 1; i < nodes.length; i++) {
                if (node == null)
                    continue;
                stuff += '<th>' + i + '</th>';
            }
            stuff += '</tr>'
            topohead.innerHTML = stuff;
            stuff = '';
            for (i = 1; i < nodes.length; i++) {
                if (node == null)
                    continue;
                stuff += '<tr><td style="vertical-align: top; text-decoration: underline; background-color: #FFFFFF;">' + i + '</td>';
                var j, k = 0;
                for (j = 1; j < nodes.length; j++) {
                    if (nodes[j] == null)
                        continue;
                    var route = routes[i];
                    if (route != undefined && k < route.length && j == route[k]) {
                        stuff += '<td>*</td>';
                        k++;
                    } else {
                        stuff += '<td>&nbsp;</td>';
                    }
                }
                stuff += '</tr>';
            }
            var topobody = document.getElementById('topobody');
            topobody.innerHTML = stuff;
        }
    }
}

function DisplayStatClass(t, n) {
    var scb = document.getElementById('statclassbody');
    var sn = document.getElementById('statnode');
    if (curclassstat != null) {
        var lastn = curclassstat.id.substr(8);
        if (n != lastn) {
            curclassstat.className = 'normal';
        }
    }
    if (curclassstat == null || t != curclassstat) {
        curclassstat = t;
        t.className = 'highlight';
        sn.style.width = '72%';
        scb.innerHTML = classstats[n];
        document.getElementById('statclass').style.display = 'block';
    } else {
        curclassstat = null;
        t.className = 'normal';
        sn.style.width = '100%';
        scb.innerHTML = '';
        document.getElementById('statclass').style.display = 'none';
    }
    return true;
}

function StatLoad(fun) {
    var params = 'fun=' + fun;
    stathttp.open('POST', 'statpost.html', true);
    stathttp.onreadystatechange = StatReply;
    stathttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    stathttp.send(params);

    return false;
}

function StatReply() {
    var xml;
    var elem;

    if (stathttp.readyState == 4 && stathttp.status == 200) {
        xml = stathttp.responseXML;
        elem = xml.getElementsByTagName('stats');
        if (elem.length > 0) {
            var errors = xml.getElementsByTagName('errors');
            var counts = xml.getElementsByTagName('counts');
            var infos = xml.getElementsByTagName('info');
            var error = errors[0];
            var cnt = error.childNodes.length;
            var count = counts[0];
            if (count.childNodes.length > cnt)
                cnt = count.childNodes.length;
            var info = infos[0];
            if (info.childNodes.length > cnt)
                cnt = info.childNodes.length;
            var stuff = '';
            var i;
            for (i = 0; i < cnt; i++) {
                if (i < error.childNodes.length)
                    if (error.childNodes[i].nodeType != 1)
                        continue;
                if (i < count.childNodes.length)
                    if (count.childNodes[i].nodeType != 1)
                        continue;
                if (i < info.childNodes.length)
                    if (info.childNodes[i].nodeType != 1)
                        continue;
                stuff += '<tr>';
                if (i < error.childNodes.length)
                    stuff += '<td style="text-align: right;">' + error.childNodes[i].getAttribute('label') + ': </td><td style="text-align: left;">' + error.childNodes[i].firstChild.nodeValue + '</td>';
                else
                    stuff += '<td>&nbsp;</td><td>&nbsp;</td>';
                if (i < count.childNodes.length)
                    stuff += '<td style="text-align: right;">' + count.childNodes[i].getAttribute('label') + ': </td><td style="text-align: left;">' + count.childNodes[i].firstChild.nodeValue + '</td>';
                else
                    stuff += '<td>&nbsp;</td><td>&nbsp;</td>';
                if (i < info.childNodes.length)
                    stuff += '<td style="text-align: right;">' + info.childNodes[i].getAttribute('label') + ': </td><td style="text-align: left;">' + info.childNodes[i].firstChild.nodeValue + '</td>';
                else
                    stuff += '<td>&nbsp;</td><td>&nbsp;</td>';
                stuff += '</tr>';
            }
            var statnetbody = document.getElementById('statnetbody');
            statnetbody.innerHTML = stuff;
            var nodes = xml.getElementsByTagName('node');
            var stuff = '';
            var oldnode = null;
            if (curclassstat != null)
                oldnode = curclassstat.id;
            for (var i = 0; i < nodes.length; i++) {
                var node = nodes[i];
                stuff += '<tr id="statnode' + i + '" onclick="return DisplayStatClass(this,' + i + ');"><td>' + node.getAttribute('id') + '</td>';
                var nstat = node.getElementsByTagName('nstat');
                for (var j = 0; j < nstat.length; j++) {
                    stuff += '<td>' + nstat[j].firstChild.nodeValue + '</td>';
                }
                stuff += '</tr>';
                var cstuff = '';
                var cclass = node.getElementsByTagName('commandclass');
                for (var j = 0; j < cclass.length; j++) {
                    cstuff += '<tr><td>' + cclass[j].getAttribute('name') + '</td>';
                    var cstat = cclass[j].getElementsByTagName('cstat');
                    for (var k = 0; k < cstat.length; k++) {
                        cstuff += '<td>' + cstat[k].firstChild.nodeValue + '</td>';
                    }
                    cstuff += '</tr>';
                }
                classstats[i] = cstuff;
            }
            var statnodebody = document.getElementById('statnodebody');
            statnodebody.innerHTML = stuff;
        }
        if (oldnode != null) {
            var scb = document.getElementById('statclassbody');
            scb.innerHTML = classstats[oldnode.substr(8)];
            curclassstat = document.getElementById(oldnode);
            curclassstat.className = 'highlight';
        }
    }
}

function TestHealLoad(fun) {
    var params = 'fun=' + fun;
    if (fun == 'test') {
        var cnt = document.getElementById('testnode');
        if (cnt.value.length == 0) {
            params += '&num=0';
        } else {
            params += '&num=' + cnt.value;
        }
        var cnt = document.getElementById('testmcnt');
        if (cnt.value.length == 0) {
            alert('Missing count value');
            return false;
        }
        params += '&cnt=' + cnt.value;
    } else if (fun == 'heal') {
        var cnt = document.getElementById('healnode');
        if (cnt.value.length == 0) {
            params += '&num=0';
        } else {
            params += '&num=' + cnt.value;
        }
        var check = document.getElementById('healrrs');
        if (check.checked)
            params += '&healrrs=1';
    }
    atsthttp.open('POST', 'thpost.html', true);
    atsthttp.onreadystatechange = TestHealReply;
    atsthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    atsthttp.send(params);

    return false;
}

function TestHealReply() {
    var xml;
    var elem;

    if (atsthttp.readyState == 4 && atsthttp.status == 200) {
        xml = atsthttp.responseXML;
        var threport = document.getElementById('testhealreport');
        elem = xml.getElementsByTagName('test');
        if (elem.length > 0) {
            threport.innerHTML = '';
        }
        elem = xml.getElementsByTagName('heal');
        if (elem.length > 0) {
            threport.innerHTML = '';
        }
    }
}

function RequestAllConfig(n) {
    var params = 'fun=racp&node=' + n;
    racphttp.open('POST', 'confparmpost.html', true);
    racphttp.onreadystatechange = PollReply;
    racphttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    racphttp.send(params);

    return false;
}

function RequestAll(n) {
    var params = 'fun=racp&node=' + n;
    racphttp.open('POST', 'refreshpost.html', true);
    racphttp.onreadystatechange = PollReply;
    racphttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    racphttp.send(params);
    return false;
}

function quotestring(s) {
    return s.replace(/\'/g, "");
}

function return2br(dataStr) {
    return dataStr.replace(/(\r\n|[\r\n])/g, "<br />");
}

function boxsize(field) {
    if (field.length < 8)
        return 8;
    return field.length + 2;
}

function CreateOnOff(i, j, vid) {
    var data = '<tr style="display:none;"><td style="float: right;"';
    var node = nodes[i];
    var value = node.values[j];
    if (value.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(value.help) + '\',0);" onmouseout="HideToolTip();"';
    data += '><label><span class="legend">' + value.label + ':&nbsp;</span></label></td><td><label class="switch"><input type="checkbox" class="val_chckbox" name="toggle_select" id="' + vid + '" onchange="return DoValue(\'' + vid + '\');"'
    if (value.readonly)
        data += ' disabled="true"';
    if (value.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(value.help) + '\',0);" onmouseout="HideToolTip();"';
    if (value.value == 'True'){
        data += ' value="off" checked';
    }
    else
        data += ' value="on"';
    data += '><div class="slider round">';
    data += '</div></label>'
    data += '</td><td><span class="legend">' + value.units + '</span></td></tr>';
    return data;
}

function CreateTextBox(i, j, vid) {
    var data = '<tr><td style="float: right;"';
    var node = nodes[i];
    var value = node.values[j];
    if (value.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(value.help) + '\',0);" onmouseout="HideToolTip();"';
    var trimmed_value = value.value.replace(/(\n\s*$)/, "");
    trimmed_value += ' ' + value.units
    data += '><label><span class="legend">' + value.label + ':&nbsp;</span></label></td><td><input type="text" class="legend form-control" size="' + boxsize(trimmed_value) + '" id="' + vid + '" value="' + trimmed_value +  '"';
    if (value.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(value.help) + '\',0);" onmouseout="HideToolTip();"';
    if (value.readonly)
        data += ' disabled="true">';
    else
        data += '>';
    if (!value.readonly)
        data += '<button class="btn btn-default" type="submit" onclick="return DoValue(\'' + vid + '\');">Submit</button>';
    data += '</td></tr>';
    return data;
}

function CreateList(i, j, vid) {
    var data = '<tr><td style="float: right;"';
    var node = nodes[i];
    var values = node.values[j];
    if (values.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(values.help) + '\',0);" onmouseout="HideToolTip();"';
    data += '><label><span class="legend">' + values.label + ':&nbsp;</span></label></td><td><select id="' + vid + '" onchange="return DoValue(\'' + vid + '\', false);"';
    if (values.help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(values.help) + '\',0);" onmouseout="HideToolTip();"';
    if (values.readonly)
        data += ' disabled="true">';
    else
        data += '>';
    if (values.value != null)
        for (k = 0; k < values.value.length; k++) {
            data += '<option value="' + values.value[k].item + '"';
            if (values.value[k].selected)
                data += ' selected="true"';
            data += '>' + values.value[k].item + '</option>';
        }
    data += '</select><span class="legend">' + values.units + '</span></td></tr>';
    return data;
}

function CreateLabel(i, j, vid) {
    return '<tr><td style="float: right;"><label><span class="legend">' + nodes[i].values[j].label + ':&nbsp;</span></label></td><td><input type="text" class="legend form-control" disabled="true" size="' + boxsize(nodes[i].values[j].value) + '" id="' + vid + '" value="' + nodes[i].values[j].value + '"><span class="legend">' + nodes[i].values[j].units + '</span></td></tr>';
}

function CreateButton(i, j, vid) {
    var data = '<tr><td style="float: right;"';
    if (nodes[i].values[j].help.length > 0)
        data += ' onmouseover="ShowToolTip(\'' + quotestring(nodes[i].values[j].help) + '\',0);" onmouseout="HideToolTip();"';
    data += '><label><span class="legend">' + nodes[i].values[j].label + ':&nbsp;</span></label></td><td><button class="btn btn-default" type="submit" id="' + vid + '" onclick="return false;" onmousedown="return DoButton(\'' + vid + '\',true);" onmouseup="return DoButton(\'' + vid + '\',false);"'
    if (nodes[i].values[j].readonly)
        data += ' disabled="true"';
    data += '>Submit</button></td><td><span class="legend">' + nodes[i].values[j].units + '</span></td></tr>';
    return data;
}

function CreateDivs(genre, divtos, ind) {
    divto = '<table border="0" cellpadding="1" cellspacing="0"><tbody>';
    var node = nodes[ind];
    if (node.values != null) {
        var j = 0;
        for (var i = 0; i < node.values.length; i++) {
            var match;
            var value = node.values[i];
            if (genre == 'user')
                match = (value.genre == genre || value.genre == 'basic');
            else
                match = (value.genre == genre);
            if (!match)
                continue;
            var vid = node.id + '-' + value.cclass + '-' + value.genre + '-' + value.type + '-' + value.instance + '-' + value.index;
            j++;
            //alert(value.toSource());
            if (value.type == 'bool') {
                divto += CreateOnOff(ind, i, vid);
            } else if (value.type == 'byte') {
                divto += CreateTextBox(ind, i, vid);
            } else if (value.type == 'int') {
                divto += CreateTextBox(ind, i, vid);
            } else if (value.type == 'short') {
                divto += CreateTextBox(ind, i, vid);
            } else if (value.type == 'decimal') {
                divto += CreateTextBox(ind, i, vid);
            } else if (value.type == 'list') {
                divto += CreateList(ind, i, vid);
            } else if (value.type == 'string') {
                divto += CreateTextBox(ind, i, vid);
            } else if (value.type == 'button') {
                divto += CreateButton(ind, i, vid);
            } else if (value.type == 'raw') {
                divto += CreateTextBox(ind, i, vid);
            }
        }
       
    }
    divtos[ind] = divto + '</tbody></table>';
}

function CreateName(val, ind) {
    nodename[ind] = '<tr><td style="float: right;"><label><span class="legend">Name:&nbsp;</span></label></td><td><input type="text" class="legend form-control" size="' + boxsize(val) + '" id="name" value="' + val + '"><button class="btn btn-default" type="submit" style="margin-left: 5px;" onclick="return DoNodePost(document.NodePost.name.value);">Submit</button></td></tr>';
}

function CreateLocation(val, ind) {
    nodeloc[ind] = '<tr><td style="float: right;"><label><span class="legend">Location:&nbsp;</span></label></td><td><input type="text" class="legend form-control" size="' + boxsize(val) + '" id="location" value="' + val + '"><button class="btn btn-default" type="submit" style="margin-left: 5px;" onclick="return DoNodePost(document.NodePost.location.value);">Submit</button></td></tr>';
}

function CreateGroup(ind) {
    var grp;
    var i, j, k;

    if (nodes[ind].groups.length == 0) {
        nodegrp[ind] = '';
        nodegrpgrp[ind] = new Array();
        nodegrpgrp[ind][1] = '';
        return;
    }
    nodegrp[ind] = '<tr><td style="float: right;"><label><span class="legend">Groups:&nbsp;</span></label></td><td><select id="group" style="margin-left: 5px;" onchange="return DoGroup();">';
    nodegrpgrp[ind] = new Array(nodes[ind].groups.length);
    grp = 1;
    for (i = 0; i < nodes[ind].groups.length; i++) {
        nodegrp[ind] += '<option value="' + nodes[ind].groups[i].id + '">' + nodes[ind].groups[i].label + ' (' + nodes[ind].groups[i].id + ')</option>';
        nodegrpgrp[ind][grp] = '<td><div id="nodegrp" name="nodegrp" style="float: right;"><select id="groups" multiple size="8" style="vertical-align: top; margin-left: 5px;">';
        k = 0;
        for (j = 1; j < nodes.length; j++) {
            var node = nodes[j];
            if (node == null)
                continue;

            // build a list of instances 
            var instances = [String(j)];
            for (var l = 0; l < node.values.length; l++) {
                instances[l + 1] = j + '.' + node.values[l].instance;
                instances.push(j + '.' + node.values[l].instance);
            }

            // make unique
            instances = instances.filter(function(item, i, ar) {
                return ar.indexOf(item) === i;
            });

            // only show when we have found multiple instances
            if (instances.length <= 2) {
                instances = [String(j)];
            }

            if (nodes[ind].groups[i].nodes != null)
                while (k < nodes[ind].groups[i].nodes.length && nodes[ind].groups[i].nodes[k] < j)
                    k++;

            for (var l = 0; l < instances.length; l++) {
                if (nodes[ind].groups[i].nodes.indexOf(instances[l]) != -1)
                    nodegrpgrp[ind][grp] += '<option selected="true">' + instances[l] + '</option>';
                else
                    nodegrpgrp[ind][grp] += '<option>' + instances[l] + '</option>';
            }
        }
        nodegrpgrp[ind][grp] += '</select></td><td><button class="btn btn-default" type="submit" style="margin-left: 5px;" onclick="return DoGrpPost();">Submit</button></div></td></tr>';
        grp++;
    }
    nodegrp[ind] += '</select></td>';
}

function CreatePoll(ind) {
    var uc = 0;
    var sc = 0;
    var node = nodes[ind];
    if (node.values != null)
        for (var i = 0; i < node.values.length; i++) {
            if (node.values[i].genre == 'user')
                uc++;
            if (node.values[i].genre == 'system')
                sc++;
        }
    if (uc > 0 || sc > 0)
        nodepoll[ind] = '<tr><td style="float: right;"><label><span class="legend">Polling&nbsp;</span></label></td><td><select id="polled" style="margin-left: 5px;" onchange="return DoPoll();"><option value="0">User</option><option value="1">System</option></select></td><td><div id="nodepoll" name="nodepoll" style="float: left;"></div></td><td><button class="btn btn-default" type="submit" style="margin-left: 5px; vertical-align: top;" onclick="return DoPollPost();">Submit</button></td></tr>';
    else
        nodepoll[ind] = '';
    nodepollpoll[ind] = new Array(2);
    CreatePollPoll('user', ind, uc);
    CreatePollPoll('system', ind, sc);
}

function CreatePollPoll(genre, ind, cnt) {
    var ind1;
    if (genre == 'user')
        ind1 = 0;
    else
        ind1 = 1;
    var nodepoll = nodepollpoll[ind];
    nodepoll[ind1] = '<select id="polls" multiple size="4" style="vertical-align: top; margin-left: 5px;">';
    if (cnt > 0) {
        var node = nodes[ind];
        for (var i = 0; i < node.values.length; i++) {
            var value = node.values[i];
            if (value.genre != genre)
                continue;
            if (value.type == 'button')
                continue;
            var vid = node.id + '-' + value.cclass + '-' + genre + '-' + value.type + '-' + value.instance + '-' + value.index;
            nodepoll[ind1] += '<option id="' + vid + '"';
            if (value.polled)
                nodepoll[ind1] += ' selected="true"';
            nodepoll[ind1] += '>' + value.label + '</option>';
        }
        nodepoll[ind1] += '</select>';
    } else
        nodepoll[ind1] = '';
}
function call_add_device(operation) {
 $('.alert-box').css("display",'');
 $('button').prop('disabled',true);
 $('input.val_chckbox').prop("disabled",true);
 $('.glyphi_icon').addClass('disabled');
 document.getElementById("adminops").value=operation;
 if (operation == 'addds' || operation == 'remd')
 {
  if (operation == 'remd')
  { $('#timer_msg i').text("remove"); }
  if (operation == 'addds')
  { $('#timer_msg i').text("add"); }
  $('#timer_msg').css('display','');
  startTimer(20,$('#timer_duration'));
 }
 DoAdmHelp();
 DoAdmPost(0);
 return false;
}
function savenickname()
{
document.getElementById("nodeops").value='nam';
DoNodePost($('#nick_name').val());
$('#'+curnode+' > td:eq(2) span').text($('#nick_name').val());
$('#modal-container-154656').modal('hide');
}
function open_modal(node_name)
{
$('#nick_name').val(node_name);
$('#modal-container-154656').modal();
}
function open_value_modal(node_id)
{
if (($(divcur[node_id]).find("tbody > tr").length) <= 1)
{
$('#tab1').html('<i class="fa fa-warning fa-lg"><span>&nbsp;&nbsp;There are no values to display for this device</span></i>');
}
else
{
$('#tab1').html(divcur[node_id]);
} 
$('#DeviceValueListDialog').modal();
}
var add_timer
function startTimer(duration, display) {
    var timer = duration, minutes, seconds;
    add_timer = setInterval(function () {
        minutes = parseInt(timer / 60, 10)
        seconds = parseInt(timer % 60, 10);

        minutes = minutes < 10 ? "0" + minutes : minutes;
        seconds = seconds < 10 ? "0" + seconds : seconds;

        display.text(minutes + ":" + seconds);

        if (--timer < 0) {
          return DoAdmPost(1);
        }
    }, 1000);
}

//arulSankar/////
  function process()
  {
        try {
        pollhttp.open("GET", 'devices', true);
        pollhttp.onreadystatechange = TestPollReply;
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            pollhttp.send(null);
        } else { // code for IE6, IE5
            pollhttp.send();
        }
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    	}catch (e) {
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    	}
	
  }

function TestPollReply() {
    var xml;
    if (pollhttp.readyState == 4 && pollhttp.status == 200) {
        clearTimeout(pollwait);
        xml = pollhttp.responseXML;
        var poll_elems = xml.getElementsByTagName('Driver');
	alert(poll_elems.length);
	var poll_Node = poll_elems[0].getElementsByTagName('Node');
	//alert(xml.getElementsByTagName('Node')[1].getAttribute('id'));
	alert(poll_Node.length);
        var poll_elem = poll_elems[0];
	var HomeID = poll_Node[3].getAttribute('id');
	alert(HomeID);
	var HomeIDXML = 'zwcfg_' + HomeID + '.xml';
        alert(HomeIDXML);
        var parameters = location.search.substring(1).split("&");
	var user = 'user-bool-1-0';
	var SendOnOff;
	var temp = parameters[0].split("=");
	//var NodeID = poll();
	//alert(NodeID);
	//var NodeID = document.getElementById(switch_binary_nodes);
	DeviceID = unescape(temp[1]);
	alert(DeviceID);
	//if ($.inArray(DeviceID, switch_binary_nodes) > -1) {alert(switch_binary_nodes.length);}
	temp = parameters[1].split("=");
	Command = unescape(temp[1]);
	alert(Command);
	temp = parameters[2].split("=");
	Action = unescape(temp[1]);
	alert(Action);
	SendOnOff = DeviceID + '-' + Command + '-' + user + '=' + Action;
	alert(SendOnOff); 
        if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
            posthttp = new XMLHttpRequest();
        } else {
            posthttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        posthttp.open('POST', 'valuepost.html', true);
        posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        posthttp.send(SendOnOff);
        //RequestAll(DeviceID);
	document.getElementById('DeviceID').innerHTML = DeviceID;
	document.getElementById('Command').innerHTML = Command;
	document.getElementById('Action').innerHTML = Action;
	return false;
    }
    return false;
}
