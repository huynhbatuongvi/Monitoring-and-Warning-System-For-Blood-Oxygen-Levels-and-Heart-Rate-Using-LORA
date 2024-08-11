// For Firebase JS SDK v7.20.0 and later, measurementId is optional
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
  
// TOGGLE SIDEBAR
// var menuBar = document.querySelector('#content nav .bx.bx-menu')	
// var sidebar = document.getElementById("sidebar")
// var k=0; 

// firebase.database().ref("/Lora").on("value",function(snapshot) {
//     if(snapshot.exists()){
//     console.log(snapshot.val())
//     var upp= snapshot.val()
//     var sidebar = document.getElementById("sidebar")
//         if (upp["menu"] == "up")	 	
//             sidebar.classList.toggle('hide',false);			
//         else 
//             sidebar.classList.toggle('hide',true);	  
//     }	
// })

// menuBar.addEventListener('click', function (){	
//     k=k+1
//     if(k%2 == 1) {
//         sidebar.classList.toggle('hide',true);
//         firebase.database().ref("/Lora").update({	"menu": "hide"})
//             }
//     else {	sidebar.classList.toggle('hide',false);
//         firebase.database().ref("/Lora").update({	"menu": "up"});
        
//     }
// })


//   const searchButton = document.querySelector('#content nav form .form-input button');
//   const searchButtonIcon = document.querySelector('#content nav form .form-input button .bx');
//   const searchForm = document.querySelector('#content nav form');

//   searchButton.addEventListener('click', function (e) {
//       if(window.innerWidth < 576) {
//           e.preventDefault();
//           searchForm.classList.toggle('show');
//           if(searchForm.classList.contains('show')) {
//               searchButtonIcon.classList.replace('bx-search', 'bx-x');
//           } else {
//               searchButtonIcon.classList.replace('bx-x', 'bx-search');
//           }
//       }
//   })





//   if(window.innerWidth < 768) {
//       sidebar.classList.add('hide');
//   } else if(window.innerWidth > 576) {
//       searchButtonIcon.classList.replace('bx-x', 'bx-search');
//       searchForm.classList.remove('show');
//   }


//   window.addEventListener('resize', function () {
//       if(this.innerWidth > 576) {
//           searchButtonIcon.classList.replace('bx-x', 'bx-search');
//           searchForm.classList.remove('show');
//       }
//   })



//   const switchMode = document.getElementById('switch-mode');

//   switchMode.addEventListener('change', function () {
//       if(this.checked) {
//           document.body.classList.add('dark');
//       } else {
//           document.body.classList.remove('dark');
//       }
//   })
//   //Nền  firebase qua WEB------------------------------------------------------
//   firebase.database().ref("/Lora").on("value",function(snapshot) {
//       if(snapshot.exists()){
//       console.log(snapshot.val())
//       var bulb_01_status = snapshot.val()
//       var input = document.getElementById("switch-mode"); 
//       if (bulb_01_status["nền"] == "Trắng")	 	{
//           input.checked=false 
//           document.body.classList.remove('dark')}
//       else 		  {
//           input.checked =true
//           document.body.classList.add('dark');
//       }
//       }
//       else
//       console.log("No data available!")
//   })
//   //Nền  web qua firebase------------------------------------------------------
  
//       var input = document.getElementById('switch-mode');
//       var outputtext = document.getElementById('status');
  
//       input.addEventListener('change',function()
//       {
//           if(this.checked) {
//               firebase.database().ref("/Lora").update({
//                   "nền": "Đen"
//               })
//               document.body.classList.add('dark');
//           } else {
//               firebase.database().ref("/Lora").update({
//                   "nền": "Trắng"})
//               document.body.classList.remove('dark')
//           }
//       });



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
        backgroundColor: 'yellow',
        borderColor: 'yellow',
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
  function updateRealtimeData() {
    firebase
      .database()
      .ref('/User/lora1/HeartRate')
      .on('value', function (snapshot) {
        const nd = snapshot.val();
        const currentTime = new Date().getTime();
        const timestamp = Math.floor(currentTime / 1000);
        heartrateData.push({ x: timestamp, y: nd });
        syncedTimestamps.push(timestamp);
  
        // Giới hạn số lượng điểm dữ liệu
        const maxDataPoints = 50;
        if (temperatureData.length > maxDataPoints) {
          heartrateData.shift();
          syncedTimestamps.shift();
        }
  
        // Cập nhật biểu đồ
        chart.update();
      });
  
    firebase
      .database()
      .ref('/User/lora1/Spo2')
      .on('value', function (snapshot) {
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

      firebase
      .database()
      .ref('/User/lora1/Temparature')
      .on('value', function (snapshot) {
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
  }
  
  // Gọi hàm cập nhật dữ liệu theo thời gian thực
  updateRealtimeData();

  
  
  
  
  
  
  

