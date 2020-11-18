let but = []; 
let mes = []; 
let min_max_aver = [];
let sch_title = [[],[]];
let defaultLang, restartMessage, restartTitle;
let parsed_day = 29;
let parsed_month = 12;
let parsed_year = 2020;
let parsed_hour = 12;

let day_from, day_to, chartDays;
let days_in_prev_month = 0;
let once = false;
let flag = 1;
let chart, rssi;
let chart_base = [[31],[11]];
let s_mode = [[],[]];
let min, max, aver, aver_sum;
let load_values = 0;
let data;
let device;

	
window.onload = function() {
	openValues();
	init();
	liveValues();
}

function chartInputChange(n){
	let num = document.getElementById("input00").value;(n>0&&num<28)?num++:(n<0&&num>2)?num--:num;
	document.getElementById("input00").value = num;
	openChart(num);
}
function changeLang(selectedLang) {event.preventDefault();openlang(selectedLang);let jsonData = 'defaultLang=' + selectedLang;let xhttp = new XMLHttpRequest();xhttp.open("GET", "lang?"+jsonData, true);xhttp.send();}
function changeZindex(n){eventType	= n; let elem = document.getElementsByClassName("tabs");elem[0].style.zIndex = n * 10;}
function add_zero(n){return n > 9 ? n: "0" + n; }

function openChart(n){
	document.getElementById("input00").value=n;
	parsed_day>=n?(day_from=parsed_day-n+1,day_to=parsed_day):(days_in_prev_month=new Date(parsed_year,parsed_month-1,0).getDate(),day_from=days_in_prev_month-n+parsed_day+1,day_to=parsed_day);

	let xhttp = new XMLHttpRequest();
	xhttp.open("GET", "configChart.json", true);
    xhttp.timeout = 10000;	
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] UpdateChart");
		openChart();
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] Reopen UpdateChart");	
		openChart();
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] UpdateChart");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let jsonfile = JSON.parse(this.responseText);
			   let jsonVal = {"date":[],"values":[]};
			   let k = day_from;
			   let time_ost = (parsed_hour / 2);	

				if (day_from > day_to) {
				k = 1;
				for (let i = day_from; i <= days_in_prev_month; i++) {
					for (let j = 0; j <= 11; j++) {
						jsonVal.date.push(add_zero(i)+"."+add_zero(parsed_month)+"."+parsed_year+"_" + add_zero(j*2)+":00");
						jsonVal.values.push(jsonfile.days[i][j]);
					}
				} 	
			}
			for (let i = k; i <= (day_to-1); i++) {
				for (let j = 0; j <= 11; j++) {
					jsonVal.date.push(add_zero(i)+"."+add_zero(parsed_month)+"."+parsed_year+"_" + add_zero(j*2)+":00");
					jsonVal.values.push(jsonfile.days[i][j]);
				}
			} 
			for (let j = 0; j <= time_ost; j++) {
				jsonVal.date.push(add_zero(parsed_day)+"."+add_zero(parsed_month)+"."+parsed_year+"_" + add_zero(j*2)+":00");
				jsonVal.values.push(jsonfile.days[day_to][j]);
			}	
			min = jsonVal.values[0];
			max = min;			
			aver_sum = 0;
			for (i in jsonVal.values) {
				if (jsonVal.values[i] > max) max = jsonVal.values[i];
				if (jsonVal.values[i] < min) min = jsonVal.values[i];
				aver_sum += parseInt(jsonVal.values[i]);
			} 
			aver = (aver_sum / jsonVal.values.length).toFixed(2);
			document.getElementById('text01').value = min_max_aver[1]+' '+min+' °C';
			document.getElementById('text02').value = min_max_aver[2]+' '+max+' °C';
			document.getElementById('text03').value = min_max_aver[3]+' '+aver+' °C';			
			data = {
				labels: jsonVal.date, 
				series: [jsonVal.values]
			};
			updateChart();
		}
	};
	xhttp.send();
}

function updateChart() {
	let options = {
		height: '400px',
		axisX: {
			showLabel: false
		},	
		chartPadding: {
			top: 15,
			right: 5,
			bottom: -25,
			left: 10
		},				
		plugins: [
			Chartist.plugins.lineTooltip({
				appendTooltipContent: function (tooltip, data, pluginOptions) {
					var title = document.createElement('div');
					title.setAttribute('class', 'ct-tooltip-title');
					title.innerHTML = data.label;
					tooltip.appendChild(title);
					var seriesLabel;
					data.series.forEach(function (value) {
						seriesLabel = pluginOptions.createSeriesLabel({
							label  : value
						}, pluginOptions);
						tooltip.appendChild(seriesLabel);
					});
				},
				createSeriesLabel: function (options, pluginOptions) {
					var seriesLabel = document.createElement('div');
					seriesLabel.setAttribute('class', 'ct-tooltip-series-label');
					seriesLabel.innerHTML = '\
					<span class="ct-tooltip-legend ct-tooltip-legend-a"></span>\
					<span class="ct-tooltip-label">\
					' + options.label + '\
					' + " °C" + '\
					</span>';
					return seriesLabel;
				}		
			})
		]
	};
	chart = new Chartist.Line('.ct-chart', data, options);
	chart.on('draw', function (data) {
		if (data.type === 'point') {
			let n = document.getElementById('input00').value;
			if (n >= 7) {
				data.element._node.setAttribute('style','stroke-width: 0px;');
			} else {
				data.element._node.setAttribute('style','stroke-width: 6px;');
			}	  
		}
	});
	var positionTooltip = function (tooltip, point) {
		tooltip.style.top  = (pointPosition.top - tooltipSize.height - 16) + 'px';
		if (point.index <= points.length / 2) {
			tooltip.style.left = (pointPosition.left - tooltipSize.width / 2 + 65) + 'px';	
			tooltip.classList.toggle('left');
		} else {
			tooltip.style.left = (pointPosition.left - tooltipSize.width / 2 - 70) + 'px';
			tooltip.classList.toggle('right');
		}
	};	
}

function openSchedule() {
	event.preventDefault();
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();
	xhttp.open("GET", 'configSetup.json', true);
    xhttp.timeout = 10000;
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] OpenSchedule");
		openSchedule();
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] Reopen OpenSchedule");	
		openSchedule();
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] OpenSchedule");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configSetup = JSON.parse(this.responseText);	
			for (let i = 0; i <= 5; ++i) {
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i];
			}
			for (let i = 0; i <= 6; i++) {
				for (let j = 0; j <= 3; j++) {
					document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value = configSetup.schedule[selIndex][i][j];
				}
			}	
		}		
	};
	xhttp.send();
}

function openlang(lng) {
    let lang_path = lng+'.json';
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();
	xhttp.open("GET", lang_path, true);
    xhttp.timeout = 10000;	
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] Openlang");
		openlang(lng);		
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] Reopen Openlang");	
		openlang(lng);
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] Openlang");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configSetup = JSON.parse(this.responseText);
			document.getElementById("text_s0").innerHTML = "&nbsp;&nbsp;&nbsp;" + configSetup.sch_title[0][0]; //Расписание светодиодов
			document.getElementById("text_s1").innerHTML = "&nbsp;&nbsp;&nbsp;" + configSetup.sch_title[0][1]; //Расписание реле

			for (let j = 0; j <= 2; j++) { 
				for (let i = 0; i <= 2; i++) {
					document.getElementById("sm"+(parseInt(i))+(parseInt(j))).innerHTML = configSetup.s_mode[j]; 
				}
			}
					
			document.getElementById('text00').innerHTML = configSetup.text[0][0];
			for (let i = 1; i <= 3; i++)  { 
				min_max_aver[i] = configSetup.text[0][i]; 
			}
			document.getElementById('text01').value = min_max_aver[1]+' -- °C';
			document.getElementById('text02').value = min_max_aver[2]+' -- °C';
			document.getElementById('text03').value = min_max_aver[3]+' -- °C';
			for (let i = 1; i <= 4; i++) {
				for (let j in configSetup.text[i])  { 
					document.getElementById('text'+(parseInt(i))+(parseInt(j))).innerHTML = configSetup.text[i][j]; 
				}
			}
			for (let i = 4; i <= 10; i++) { document.getElementById('input3'+(parseInt(i))).title = configSetup.popup[1];   } //Требуется перезагрузка
			for (let i in configSetup.but) { but[i] = configSetup.but[i]; } //Сообщения кнопок
			for (let i in configSetup.mes) { mes[i] = configSetup.mes[i]; } //Сообщения ошибок
			for (let i = 0; i <= 5; i++) { //Заголовки расписания
				sch_title[0][i] = configSetup.sch_title[1][i];
				sch_title[1][i] = configSetup.sch_title[2][i];
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i];
			}
			restartMessage = configSetup.popup[0]; //Перезагрузить ESP8266-12E?
			document.getElementById("Button1").value = but[5]; //Сохранить
			document.getElementById("Button2").value = but[5]; //Сохранить
			document.getElementById("Button3").value = but[5]; //Сохранить
			document.getElementById("Button4").value = but[5]; //Сохранить
			document.getElementById("Button5").value = but[5]; //Сохранить			
			document.getElementById("Button6").value = but[3]; //Ручной ввод даты			
			document.getElementById("Button7").value = but[4]; //Автонастройка даты
			document.getElementById("Button8").value = but[5]; //Сохранить		
			document.getElementById("Button9").innerHTML = but[6]; //Открыть
			document.getElementById("Button10").value = but[7]; //Загрузить
			document.getElementById("Button11").value = but[8]; //Перезагрузить
		}
	};
	xhttp.send();	
}

function openValues() {
    let selectList = document.getElementById("select_lang");
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();

    xhttp.open("GET", 'configSetup.json', true);
    xhttp.timeout = 10000;
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] OpenValues");
		openValues();
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] OpenValues");	
		openValues();
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] OpenValues");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configSetup = JSON.parse(this.responseText);
			for (let i in configSetup.lang){
				let option = document.createElement("option");
				option.value = configSetup.lang[i];
				option.text = configSetup.lang[i];
				selectList.appendChild(option);
			}
			document.getElementById("text_t1").innerText='ver: ' + configSetup.ver;
			selectList.value = configSetup.defaultLang;
			chartDays = configSetup.input[0][0]; //Количество дней в графике
			load_values = 1;
			openlang(selectList.value);
			for (let i = 0; i <= 2; i++) { //Режимы реле Авто Вкл Выкл
				document.getElementById('select_smode'+(parseInt(i))).value = configSetup.s_mode[i];	
			}    
			for (let i = 0; i <= 3; i++) { //Все инпуты
				for (let j in configSetup.input[i]) { 
					document.getElementById('input'+(parseInt(i))+(parseInt(j))).value = configSetup.input[i][j]; 
				}
			}
			for (let i = 0; i <= 5; i++) {
			document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i] 
			}
			for (let i = 0; i <= 6; i++) { 																							 
				for (let j = 0; j <= 3; j++) {
					document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value = configSetup.schedule[selIndex][i][j];
				}
			}
		}
	};
xhttp.send();
}

function ButtonClick(buttonNumb, method, url, setText, setBody) {
	let message = new Object();	
	message.loading = but[0];
	message.success = but[1];
	message.failure = but[2];
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			buttonNumb.value = message.success;
			buttonNumb.setAttribute("class", "btn btn-success");	
			setTimeout(() => { 
				buttonNumb.value = setText;
				buttonNumb.setAttribute("class", "btn btn-primary");			
			}, 1000);
		} else if (this.readyState < 4){
			buttonNumb.value = message.loading;
			buttonNumb.setAttribute("class", "btn btn-warning");	
		} else {
			buttonNumb.value = message.failure;
			buttonNumb.setAttribute("class", "btn btn-danger");	
			setTimeout(() => { 
				buttonNumb.setAttribute("class", "btn btn-primary");
				buttonNumb.value = setText;
			}, 1000);
		}
	};
	xhttp.open(method, url, true);
    xhttp.timeout = 10000;
	xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    method == "GET"?xhttp.send():xhttp.send(setBody);
}

function init(){
	let button1 = document.getElementById("Button1");
	let button2 = document.getElementById("Button2");
	let button3 = document.getElementById("Button3");
	let button4 = document.getElementById("Button4");
	let button5 = document.getElementById("Button5");
	let button6 = document.getElementById("Button6");   
	let button7 = document.getElementById("Button7");
	let button8 = document.getElementById("Button8");
	let button9 = document.getElementById("Button9");
	let button10 = document.getElementById("Button10");
	let button11 = document.getElementById("Button11");	
	button1.addEventListener("click", function(event){ 
		event.preventDefault(); 
		chartDays = document.getElementById("input00").value;
		let jsonData = 'input[0][0]=' + chartDays;
		ButtonClick(button1, "GET", 'chartDays?'+jsonData, but[5], ""); 
	});
	button2.addEventListener("click", function(event){ 
		let sched = [[],[],[],[],[],[],[],[]];
		let selIndex = document.getElementById("select_schedule").options.selectedIndex;
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 6; i++) {
			for (let j = 0; j <= 3; j++) {
				sched[i][j] = document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value;
				jsonData += 'schedule['+selIndex+']['+i+']['+j+']='+sched[i][j]+'&';
			}
		}
		ButtonClick(button2, "GET", 'save_schedule?selIndex='+selIndex+'&'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});
	button3.addEventListener("click", function(event){ 
	 	let selIndex0 = document.getElementById("select_smode0").options.selectedIndex;
	 	let selIndex1 = document.getElementById("select_smode1").options.selectedIndex;
	 	let selIndex2 = document.getElementById("select_smode2").options.selectedIndex;		
		let jsonData = '';
		event.preventDefault(); 
 		for (let i = 0; i <= 2; i++) {
			jsonData += 's_mode['+(parseInt(i))+']='+document.getElementById("select_smode"+(parseInt(i))).options.selectedIndex+'&';
		} 
		ButtonClick(button3, "GET", 's_mode?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button4.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 6; i++) {
			jsonData += 'input[1]['+i+']='+document.getElementById('input1'+(parseInt(i))).value+'&';
		}
		ButtonClick(button4, "GET", 'save?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button5.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 5; i++) {
			jsonData += 'input[2]['+i+']='+document.getElementById('input2'+(parseInt(i))).value+'&';
		}
		ButtonClick(button5, "GET", 'mqtt?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button6.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'input[3][0]=' + document.getElementById("input30").value + '&input[3][1]=' + document.getElementById("input31").value;
		ButtonClick(button6, "GET", 'save_date?'+jsonData, but[3], ""); 
	});
	button7.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'input[3][2]=' + document.getElementById("input32").value + '&input[3][3]=' + document.getElementById("input33").value;
		ButtonClick(button7, "GET", 'auto_sync?'+jsonData, but[4], ""); 
	});		
	button8.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
  		for (let i = 4; i <= 10; i++) {
			jsonData += 'input[3]['+i+']='+document.getElementById('input3'+(parseInt(i))).value+'&';
		} 
		ButtonClick(button8, "POST", '/ssid', but[5], jsonData.substring(0, jsonData.length-1)); 
	});	
	
	button11.addEventListener("click", function(event){ 
		event.preventDefault(); 
        if (device==1) {
		    ButtonClick(button11, "POST", '/restart', but[8], 'device='+device); 		
			setTimeout(() => {window.location.reload(true);}, 10000);
		} 
	});
}

function liveValues() {
	//var ws = new WebSocket("ws://" + window.location.hostname + "/ws");
	var ws = new ReconnectingWebSocket("ws://" + window.location.hostname + "/ws", null, {debug: true, reconnectInterval: 3000, timeoutInterval: 3000});
    //ws = new WebSocket("ws://echo.websocket.org");	
	ws.onopen = function() {
		console.log("[WEBSOCKET] Websocket-соединение установлено...");
	};
    ws.onmessage = function(event) {
		//console.log("[message] Данные с сервера получены");
		let json = JSON.parse(event.data);
        let fan  = !(json.bf&32)?'0':'1'; // fan  00100000 32
		let ten  = !(json.bf&16)?'0':'1'; // ten  00010000 16
		let rel1 = !(json.bf&8)?'0':'1';  // rel1 00001000 8
		let rel2 = !(json.bf&4)?'0':'1';  // rel2 00000100 4
		let mqtt = !(json.bf&2)?'0':'1';  // mqtt 00000010 2
		let ws_client = json.clws;
		
        if (json.now == ""){
			document.getElementById("text_t3").innerHTML = mes[1];
			document.getElementById("text_t3").style="color:red;font-size:20px;width:65%;text-align:center;vertical-align:middle;display:table-cell;";
			//console.log(json.now);
		} else {
			let dtnow = new Date(json.now);
			parsed_year = dtnow.getFullYear();
			parsed_month = dtnow.getMonth() + 1;
			parsed_day = dtnow.getDate();
			parsed_hour = dtnow.getHours();	
			//if(!once && load_values) {setTimeout(()=>{openChart(chartDays);},1000); once = true;}
			if(!once && load_values) {openChart(chartDays); once = true;}
			let ostatok = parsed_hour % 2;
            (ostatok == 0 && flag == 0)?(num=document.getElementById("input00").value,flag=1,setTimeout(()=>{openChart(num);},3000)):(ostatok != 0)?flag=0:flag; //обновление графика после появления новой точки
			document.getElementById("text_t3").innerHTML = dtnow.toLocaleString();
            document.getElementById("text_t3").style="color:white;font-size:20px;line-height:20px;text-align:center;font-weight:bold;";
		}
		
		
        class_wifi = json.rssi<=-110?'wifi1 mx-auto d-block':json.rssi<=-100?'wifi2 mx-auto d-block':json.rssi<=-85?'wifi3 mx-auto d-block':json.rssi<=-70?'wifi4 mx-auto d-block':json.rssi<=-50?'wifi5 mx-auto d-block':'wifi6 mx-auto d-block';
        class_led = json.led === 0 ? 'led_0 mx-auto d-block' : 'led_1 mx-auto d-block';
		document.getElementById("text_t5").innerHTML = json.led+"%";	
		class_fan = 'fan_' + fan +'  mx-auto d-block';
		class_ten = 'ten_' + ten +'  mx-auto d-block';
		class_relay1 = 'relay1_' + rel1 +'  mx-auto d-block';
		class_relay2 = 'relay2_' + rel2 +'  mx-auto d-block';
        document.getElementById("Picture2").setAttribute("class", class_wifi);
        document.getElementById("Picture3").setAttribute("class", class_led);
        document.getElementById("Picture4").setAttribute("class", class_fan);        
		document.getElementById("Picture5").setAttribute("class", class_ten);	
        document.getElementById("Picture6").setAttribute("class", class_relay1);        
		document.getElementById("Picture7").setAttribute("class", class_relay2);	
		rssi = 'RSSI: '+json.rssi+' dBm\n'+'MQTT: '+mqtt+ ' WS: ' +ws_client+ '\n';
		document.getElementById("Picture2").setAttribute('data-tooltip',rssi);
	    document.getElementById("text_t2").innerHTML = json.t;	
		if (json.t == 85.0 || json.t == -127 || json.t > 150) {
			document.getElementById("text_t2").innerHTML = mes[0];		
		    document.getElementById("Picture1").setAttribute("class", "tempna  mx-auto d-block");	
		} else {
			document.getElementById("text_t2").innerHTML = json.t + "°C";
		    document.getElementById("Picture1").setAttribute("class", "temp  mx-auto d-block");
		}
	};
    ws.onclose = function(event) {
		ws = null;
		console.log("[WEBSOCKET] Websocket-соединение разорвано. Повторное подключение...");
	    //setTimeout(function() { liveValues(); }, 1000);	
		
    };	
    ws.onerror = function(event) {
		console.log("[WEBSOCKET] Ошибка подключения...Повторное подключение...");
		//ws.close();
    };
};

let eventType = 0;	
document.addEventListener('touchstart', handleTouchStart, false);
document.addEventListener('touchmove', handleTouchMove, false);
document.addEventListener('touchend', handleTouchEnd, false);
let xDown = null;
let yDown = null;
let xDiff = null;
let yDiff = null;
let timeDown = null;
let startEl = null;
function handleTouchEnd(e) {
    if (startEl !== e.target) return;
    let swipeThreshold = 200; /* расстояние касания default 10px */
    let swipeTimeout = 1000;  /* время касания default 1000ms    */
    let timeDiff = Date.now() - timeDown;
    if (Math.abs(xDiff) > Math.abs(yDiff)) { // most significant
        if (Math.abs(xDiff) > swipeThreshold && timeDiff < swipeTimeout) {
            if (xDiff > 0) {
                eventType ++;				
				if (eventType > 4) eventType = 0;
            }
            else {
                eventType --;	
				if (eventType < 0) eventType = 4;
            }
        }
        changeZindex(eventType);
		document.getElementById('tab_'+ eventType).checked = true;		
		if (eventType==0) {updateChart();} //openChart(chartDays);		
	}		
    if (eventType !== '') {
        startEl.dispatchEvent(new CustomEvent(eventType, { bubbles: true, cancelable: true }));
    }
    xDown = null;
    yDown = null;
    timeDown = null;
};
function handleTouchStart(e){if (e.target.getAttribute('data-swipe-ignore') === 'true') return;startEl = e.target;timeDown = Date.now();xDown = e.touches[0].clientX;yDown = e.touches[0].clientY;xDiff = 0;yDiff = 0;}
function handleTouchMove(e) {if (!xDown || !yDown) return;let xUp = e.touches[0].clientX;let yUp = e.touches[0].clientY;xDiff = xDown - xUp;yDiff = yDown - yUp;}