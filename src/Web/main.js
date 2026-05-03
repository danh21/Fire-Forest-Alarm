// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
    apiKey: "AIzaSyAQ2lAaB841rSTSNE4aEhDJL9C466XpCAo",
    authDomain: "prj2-forestfirealarm.firebaseapp.com",
    databaseURL: "https://prj2-forestfirealarm-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "prj2-forestfirealarm",
    storageBucket: "prj2-forestfirealarm.appspot.com",
    messagingSenderId: "246319704653",
    appId: "1:246319704653:web:e95e40f9866d4325b6ca04",
    measurementId: "G-7NYRJ0Z9HE"
};
// Initialize Firebase
firebase.initializeApp(firebaseConfig);
firebase.analytics();
 
function updateStt(loc, area, stt, date) {
    loc.on("value", function(snapshot) {
        snapshot.forEach(function(child) {
            stt.innerText = child.child('status').val()
            var d = new Date()
            date.innerHTML = d.toLocaleString();
            if (stt.innerHTML == 'normal') {
                area.classList.remove("warningArea")
                area.classList.add("normalArea") 
            }
            else {
                area.classList.remove("normalArea")
                area.classList.add("warningArea")
            }      
        });
    });
}

var stt1 = document.getElementById('stt1');
var area1 = document.getElementById('area1');
var date1 = document.getElementById('date1');
var loc1 = firebase.database().ref().child('Location 1');
updateStt(loc1, area1, stt1, date1)

var stt2 = document.getElementById('stt2');
var area2 = document.getElementById('area2');
var date2 = document.getElementById('date2');
var loc2 = firebase.database().ref().child('Location 2');
updateStt(loc2, area2, stt2, date2)

var stt3 = document.getElementById('stt3');
var area3 = document.getElementById('area3');
var date3 = document.getElementById('date3');
var loc3 = firebase.database().ref().child('Location 3');
updateStt(loc3, area3, stt3, date3)

var stt4 = document.getElementById('stt4');
var area4 = document.getElementById('area4');
var date4 = document.getElementById('date4');
var loc4 = firebase.database().ref().child('Location 4');
updateStt(loc4, area4, stt4, date4)