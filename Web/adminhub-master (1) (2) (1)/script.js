const firebaseConfig = {
	apiKey: "AIzaSyChEv8Xn7_nYAp3k0IUmPNId0qIhoxl9MI",
	authDomain: "heartrate-2cd32.firebaseapp.com",
	databaseURL: "https://heartrate-2cd32-default-rtdb.asia-southeast1.firebasedatabase.app",
	projectId: "heartrate-2cd32",
	storageBucket: "heartrate-2cd32.appspot.com",
	messagingSenderId: "156448670144",
	appId: "1:156448670144:web:5fefe65df007c009598825",
	measurementId: "G-07XG3K5JCG"
  };
	firebase.initializeApp(firebaseConfig);
	var database = firebase.database();


	const allSideMenu = document.querySelectorAll('#sidebar .side-menu.top li a');

	allSideMenu.forEach(item=> {
		const li = item.parentElement;

		item.addEventListener('click', function () {
			allSideMenu.forEach(i=> {
				i.parentElement.classList.remove('active');
			})
			li.classList.add('active');
		})
	});
	// TOGGLE SIDEBAR
	//var menuBar = document.querySelector('#content nav .bx.bx-menu')	
	//var sidebar = document.getElementById("sidebar")
	// var k=0; 

	// firebase.database().ref("/Lora").on("value",function(snapshot) {
	// 	if(snapshot.exists()){
	// 	console.log(snapshot.val())
	// 	var upp= snapshot.val()
	// 	var sidebar = document.getElementById("sidebar")
	// 		if (upp["menu"] == "up")	 	
	// 			sidebar.classList.toggle('hide',false);			
	// 		else 
	// 			sidebar.classList.toggle('hide',true);	  
	// 	}	
	// })
	
	// menuBar.addEventListener('click', function (){	
	// 	k=k+1
	// 	if(k%2 == 1) {
	// 		sidebar.classList.toggle('hide',true);
	// 		firebase.database().ref("/Lora").update({	"menu": "hide"})
	// 			}
	// 	else {	sidebar.classList.toggle('hide',false);
	// 		firebase.database().ref("/Lora").update({	"menu": "up"});
			
	// 	}
	// })


	// const searchButton = document.querySelector('#content nav form .form-input button');
	// const searchButtonIcon = document.querySelector('#content nav form .form-input button .bx');
	// const searchForm = document.querySelector('#content nav form');

	// searchButton.addEventListener('click', function (e) {
	// 	if(window.innerWidth < 576) {
	// 		e.preventDefault();
	// 		searchForm.classList.toggle('show');
	// 		if(searchForm.classList.contains('show')) {
	// 			searchButtonIcon.classList.replace('bx-search', 'bx-x');
	// 		} else {
	// 			searchButtonIcon.classList.replace('bx-x', 'bx-search');
	// 		}
	// 	}
	// })





	// if(window.innerWidth < 768) {
	// 	sidebar.classList.add('hide');
	// } else if(window.innerWidth > 576) {
	// 	searchButtonIcon.classList.replace('bx-x', 'bx-search');
	// 	searchForm.classList.remove('show');
	// }


	// window.addEventListener('resize', function () {
	// 	if(this.innerWidth > 576) {
	// 		searchButtonIcon.classList.replace('bx-x', 'bx-search');
	// 		searchForm.classList.remove('show');
	// 	}
	// })



	// const switchMode = document.getElementById('switch-mode');

	// switchMode.addEventListener('change', function () {
	// 	if(this.checked) {
	// 		document.body.classList.add('dark');
	// 	} else {
	// 		document.body.classList.remove('dark');
	// 	}
	// })
	//Nền  firebase qua WEB------------------------------------------------------
	firebase.database().ref("/User/lora1").on("value",function(snapshot) {
		if(snapshot.exists()){
		console.log(snapshot.val())
		var bulb_01_status = snapshot.val()
		var input = document.getElementById("switch-mode"); 
		if (bulb_01_status["nền"] == "Trắng")	 	{
			input.checked=false 
			document.body.classList.remove('dark')}
		else 		  {
			input.checked =true
			document.body.classList.add('dark');
		}
		}
		else
		console.log("No data available!")
	})
	



	// Auto load Temperature-------------------------
	firebase.database().ref("/User/lora1/HeartRate").on("value",function(snapshot){
	var nd = snapshot.val();  
	var patientBox = document.getElementById("patient1-box");
	document.getElementById("HeartRate").innerHTML = nd;
	console.log(nd);

	if (nd < 40 || nd > 120) {
        patientBox.classList.add("blink");
    } else {
        patientBox.classList.remove("blink");
    }
	});

	firebase.database().ref("/User/lora1/Spo2").on("value",function(snapshot){
	var nd = snapshot.val();  
	var patientBox = document.getElementById("patient1-box");
	document.getElementById("Spo2").innerHTML = nd;
	console.log(nd);
	if (nd < 90) {
        patientBox.classList.add("blink");
    } else {
        patientBox.classList.remove("blink");
    }
	});
	firebase.database().ref("/User/lora1/Temp").on("value",function(snapshot){
		var nd = snapshot.val();  
		var patientBox = document.getElementById("patient1-box");
		document.getElementById("Nhietdo").innerHTML = nd;
		console.log(nd);
		if (nd <= 36 || nd >= 38) {
			patientBox.classList.add("blink");
		} else {
			patientBox.classList.remove("blink");
		}
	});

	firebase.database().ref("/User/lora2/HeartRate").on("value",function(snapshot){
		var nd = snapshot.val();  
		var patientBox = document.getElementById("patient2-box");
		document.getElementById("HeartRate2").innerHTML = nd;
		console.log(nd);
		if (nd < 40 || nd > 120) {
			patientBox.classList.add("blink");
		} else {
			patientBox.classList.remove("blink");
		}
		});
		

		firebase.database().ref("/User/lora2/Spo2").on("value",function(snapshot){
			var nd = snapshot.val();  
			var patientBox = document.getElementById("patient2-box");
			document.getElementById("Spo2-2").innerHTML = nd;
			console.log(nd);
			if (nd < 90) {
				patientBox.classList.add("blink");
			} else {
				patientBox.classList.remove("blink");
			}
			});
		firebase.database().ref("/User/lora2/Temp").on("value",function(snapshot){
				var nd = snapshot.val();
				var patientBox = document.getElementById("patient2-box");  
				document.getElementById("Nhietdo2").innerHTML = nd;
				console.log(nd);
				if (nd <= 35 || nd >= 38) {
					patientBox.classList.add("blink");
				} else {
					patientBox.classList.remove("blink");
				}
			});
	

			firebase.database().ref("/User/lora1/Thông tin/Name").on("value", function(snapshot) {
				var name = snapshot.val();
				document.getElementById("patient-name").innerHTML = name;
				console.log(nd);
			});
			
			firebase.database().ref("/User/lora1/Thông tin/Birth").on("value", function(snapshot) {
				var age = snapshot.val();
				document.getElementById("patient-age").innerHTML = age;
				console.log(nd);
			});
		
			firebase.database().ref("/User/lora2/Thông tin/Name").on("value", function(snapshot) {
				var name = snapshot.val();
				document.getElementById("patient-name-2").innerHTML = name;
				console.log(nd);
			});
			
			firebase.database().ref("/User/lora2/Thông tin/Birth").on("value", function(snapshot) {
				var age = snapshot.val();
				document.getElementById("patient-age-2").innerHTML = age;
				console.log(nd);
			});
		
	
	let activePatient = null;

	
	

document.querySelectorAll('.box-info li').forEach(item => {
    item.addEventListener('click', function() {
        const patientId = this.getAttribute('data-patient');
        const patientInfo = document.getElementById(patientId);
        
        if (activePatient === patientId) {
            // Nếu tab hiện tại đang được hiển thị, ẩn đi khi nhấn lại
            patientInfo.classList.remove('active');
            activePatient = null;
        } else {
            // Nếu tab hiện tại không được hiển thị, hiển thị khi nhấn và ẩn tab hiện tại khác (nếu có)
            document.querySelectorAll('.patient-info').forEach(info => {
                if (info.id === patientId) {
                    info.classList.add('active');
                    activePatient = patientId;
                } else {
                    info.classList.remove('active');
                }
            });
        }
    });
});






	const labels = [];
	const heartrateData = [];
	const temperatureData = [];
	const spo2Data = [];
	const syncedTimestamps = [];
	const data = {
	  labels: labels,
	  datasets: [
		{
		  label: 'Heart rate',
		  backgroundColor: 'red',
		  borderColor: 'red',
		  borderWidth: 1,
		  fill: false,
		  data: heartrateData,
		  tension: 0.4,
		},
		{
		  label: 'Spo2',
		  backgroundColor: 'purple',
		  borderColor: 'purple',
		  borderWidth: 1,
		  fill: false,
		  data: spo2Data,
		  tension: 0.4,
		},
		{
		  label: 'Temperature',
		  backgroundColor: 'blue',
		  borderColor: 'blue',
		  borderWidth: 1,
		  fill: false,
		  data: temperatureData,
		  tension: 0.4,
		},
	  ],
	};
	
	const options = {
	  responsive: true,
	  title: {
		display: true,
		text: 'Realtime HeartRate and SpO2',
	  },
	  scales: {
		x: {
		  type: 'linear',
		  ticks: {
			stepSize: 1,
			callback: function (value, index, values) {
			  const currentTime = new Date().getTime();
			  const timestamp = currentTime - ((values.length - index) * 1000);
			  const date = new Date(timestamp);
			  const hours = date.getHours().toString().padStart(2, '0');
			  const minutes = date.getMinutes().toString().padStart(2, '0');
			  const seconds = date.getSeconds().toString().padStart(2, '0');
			  return hours + ':' + minutes + ':' + seconds;
			},
		  },
		},
		y: {
		  ticks: {
			beginAtZero: true,
		  },
		},
	  },
	};
	
	const canvas = document.getElementById('canvas');
	const chart = new Chart(canvas, {
	  type: 'line',
	  data: data,
	  options: options,
	});
	
	// Cập nhật dữ liệu theo thời gian thực
	
	// Cập nhật dữ liệu theo thời gian thực
// Cập nhật dữ liệu theo thời gian thực
function updateRealtimeData() {
    firebase.database().ref('/User/lora1/HeartRate').on('value', function(snapshot) {
        const nd = snapshot.val();
        const currentTime = new Date().getTime();
        const timestamp = Math.floor(currentTime / 1000);
        heartrateData.push({ x: timestamp, y: nd });
        syncedTimestamps.push(timestamp);

        // Giới hạn số lượng điểm dữ liệu
        const maxDataPoints = 50;
        if (heartrateData.length > maxDataPoints) {
            heartrateData.shift();
            syncedTimestamps.shift();
        }

        // Cập nhật biểu đồ
        chart.update();
    });

    firebase.database().ref('/User/lora1/Spo2').on('value', function(snapshot) {
        const nd = snapshot.val();
        const currentTime = new Date().getTime();
        const timestamp = Math.floor(currentTime / 1000);
        spo2Data.push({ x: timestamp, y: nd });
        syncedTimestamps.push(timestamp);

        // Giới hạn số lượng điểm dữ liệu
        const maxDataPoints = 50;
        if (spo2Data.length > maxDataPoints) {
            spo2Data.shift();
            syncedTimestamps.shift();
        }

        // Cập nhật biểu đồ
        chart.update();
    });

    firebase.database().ref('/User/lora1/Temp').on('value', function(snapshot) {
        const nd = snapshot.val();
        const currentTime = new Date().getTime();
        const timestamp = Math.floor(currentTime / 1000);
        temperatureData.push({ x: timestamp, y: nd });
        syncedTimestamps.push(timestamp);

        // Giới hạn số lượng điểm dữ liệu
        const maxDataPoints = 50;
        if (temperatureData.length > maxDataPoints) {
            temperatureData.shift();
            syncedTimestamps.shift();
        }

        // Cập nhật biểu đồ
        chart.update();
    });
}


	// Gọi hàm cập nhật dữ liệu theo thời gian thực
	updateRealtimeData();

// Lắng nghe sự kiện thay đổi từ Firebase và cập nhật dữ liệu
// firebase.database().ref('/User/lora1/HeartRate').on('value', function(snapshot) {
//     const nd = snapshot.val();
//     const currentTime = new Date().getTime();
//     const timestamp = Math.floor(currentTime / 1000);
//     heartrateData.push({ x: timestamp, y: nd });

//     // Giới hạn số lượng điểm dữ liệu
//     const maxDataPoints = 100;
//     if (heartrateData.length > maxDataPoints) {
//         heartrateData.shift();
//     }

//     // Cập nhật biểu đồ
//     chart.update();
// });

// firebase.database().ref('/User/lora1/Spo2').on('value', function(snapshot) {
//     const nd = snapshot.val();
//     const currentTime = new Date().getTime();
//     const timestamp = Math.floor(currentTime / 1000);
//     spo2Data.push({ x: timestamp, y: nd });

//     // Giới hạn số lượng điểm dữ liệu
//     const maxDataPoints = 100;
//     if (spo2Data.length > maxDataPoints) {
//         spo2Data.shift();
//     }

//     // Cập nhật biểu đồ
//     chart.update();
// });

// firebase.database().ref('/User/lora1/Temp').on('value', function(snapshot) {
//     const nd = snapshot.val();
//     const currentTime = new Date().getTime();
//     const timestamp = Math.floor(currentTime / 1000);
//     temperatureData.push({ x: timestamp, y: nd });

//     // Giới hạn số lượng điểm dữ liệu
//     const maxDataPoints = 100;
//     if (temperatureData.length > maxDataPoints) {
//         temperatureData.shift();
//     }

//     // Cập nhật biểu đồ
//     chart.update();
// });

	// Khởi tạo biến lưu trữ dữ liệu và nhãn cho biểu đồ thứ hai
const labels2 = [];
const heartrateData2 = [];
const temperatureData2 = [];
const spo2Data2 = [];
const syncedTimestamps2 = [];

// Khởi tạo đối tượng biểu đồ thứ hai
const data2 = {
    labels: labels2,
    datasets: [
        {
            label: 'Heart rate',
            backgroundColor: 'red',
            borderColor: 'red',
            borderWidth: 1,
            fill: false,
            data: heartrateData2,
            tension: 0.4,
        },
        {
            label: 'Spo2',
            backgroundColor: 'purple',
            borderColor: 'purple',
            borderWidth: 1,
            fill: false,
            data: spo2Data2,
            tension: 0.4,
        },
        {
            label: 'Temperature',
            backgroundColor: 'blue',
            borderColor: 'blue',
            borderWidth: 1,
            fill: false,
            data: temperatureData2,
            tension: 0.4,
        },
    ],
};

const options2 = {
    responsive: true,
    title: {
        display: true,
        text: 'Realtime HeartRate, SpO2, and Temperature for Patient 2',
    },
    scales: {
        x: {
            type: 'linear',
            ticks: {
                stepSize: 1,
                callback: function (value, index, values) {
                    const currentTime = new Date().getTime();
                    const timestamp = currentTime - ((values.length - index) * 1000);
                    const date = new Date(timestamp);
                    const hours = date.getHours().toString().padStart(2, '0');
                    const minutes = date.getMinutes().toString().padStart(2, '0');
                    const seconds = date.getSeconds().toString().padStart(2, '0');
                    return hours + ':' + minutes + ':' + seconds;
                },
            },
        },
        y: {
            ticks: {
                beginAtZero: true,
            },
        },
    },
};

// Khởi tạo biểu đồ thứ hai trên canvas2
const canvas2 = document.getElementById('canvas1');
const chart2 = new Chart(canvas2, {
    type: 'line',
    data: data2,
    options: options2,
});

// Hàm cập nhật dữ liệu thời gian thực cho bệnh nhân thứ hai
function updateRealtimeData2() {
    firebase
        .database()
        .ref('/User/lora2/HeartRate')
        .on('value', function (snapshot) {
            const nd = snapshot.val();
            const currentTime = new Date().getTime();
            const timestamp = Math.floor(currentTime / 1000);
            heartrateData2.push({ x: timestamp, y: nd });
            syncedTimestamps2.push(timestamp);

            // Giới hạn số lượng điểm dữ liệu
            const maxDataPoints = 100;
            if (heartrateData2.length > maxDataPoints) {
                heartrateData2.shift();
                syncedTimestamps2.shift();
            }

            // Cập nhật biểu đồ
            chart2.update();
        });

    firebase
        .database()
        .ref('/User/lora2/Spo2')
        .on('value', function (snapshot) {
            const nd = snapshot.val();
            const currentTime = new Date().getTime();
            const timestamp = Math.floor(currentTime / 1000);
            spo2Data2.push({ x: timestamp, y: nd });
            syncedTimestamps2.push(timestamp);
            // Giới hạn số lượng điểm dữ liệu
            const maxDataPoints = 100;
            if (spo2Data2.length > maxDataPoints) {
                spo2Data2.shift();
                syncedTimestamps2.shift();
            }

            // Cập nhật biểu đồ
            chart2.update();
        });

    firebase
        .database()
        .ref('/User/lora2/Temp')
        .on('value', function (snapshot) {
            const nd = snapshot.val();
            const currentTime = new Date().getTime();
            const timestamp = Math.floor(currentTime / 1000);
            temperatureData2.push({ x: timestamp, y: nd });
            syncedTimestamps2.push(timestamp);
            // Giới hạn số lượng điểm dữ liệu
            const maxDataPoints = 100;
            if (temperatureData2.length > maxDataPoints) {
                temperatureData2.shift();
                syncedTimestamps2.shift();
            }

            // Cập nhật biểu đồ
            chart2.update();
        });
}

// Gọi hàm cập nhật dữ liệu thời gian thực cho bệnh nhân thứ hai
updateRealtimeData2();




	
	
	
	

	