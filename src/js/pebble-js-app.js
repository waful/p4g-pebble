var CONDITION_KEY = {
    clear: 0,
    cloudy: 2,
    rain: 3,
    snow: 4,
    thunder: 5,
    fog: 6,
    unknown: 7,
    blank: 8
};

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    console.log("coords: " + pos.coords.latitude + ", " + pos.coords.longitude);
    console.log("accuracy: " + pos.coords.accuracy);
    
    var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
        pos.coords.latitude + "&lon=" + pos.coords.longitude;

    xhrRequest(url, 'GET',
        function (responseText) {
            var json = JSON.parse(responseText);
            console.log("server says: " + JSON.stringify(json));

            var conditionId = parseInt(json.weather[0].id);
            var conditions;
            switch(parseInt(conditionId/100)){
                case 8:
                    if(conditionId === 800){
                        conditions = CONDITION_KEY.clear;
                    }
                    else{
                        conditions = CONDITION_KEY.cloudy;
                    }
                    break;
                case 2:
                    conditions = CONDITION_KEY.thunder;
                    break;
                case 3:
                case 4:
                    conditions = CONDITION_KEY.rain;
                    break;
                case 6:
                    conditions = CONDITION_KEY.snow;
                    break;
                case 7:
                    conditions = CONDITION_KEY.fog;
                    break;
                case 9:
                    conditions = CONDITION_KEY.unknown;
                    break;
                default:
                    conditions = CONDITION_KEY.blank;
                    break;
            }
            localStorage.setItem("last_fetch_date", new Date());
            localStorage.setItem("last_fetch_conditions", conditions);
            localStorage.setItem("last_fetch_details", conditionId + " " + json.weather[0].main);
            sendToApp(conditions);
        }
    );
}

function locationError(err) {
    console.log("could not get location");
}

function getWeather() {
    // use cache if last fetch is younger than 15 minutes
    var lastFetchDate = Date.parse(localStorage.getItem("last_fetch_date"));
    var lastFetchConditions = Number.parseInt(localStorage.getItem("last_fetch_conditions"));
    if(lastFetchDate
       && lastFetchConditions >= 0){
        console.log("using cached conditions: " + lastFetchConditions + " " + localStorage.getItem("last_fetch_details"));
        sendToApp(lastFetchConditions);
        if(new Date() - lastFetchDate >= 900000){
            getNewWeather();
        }
    }
    else{
        getNewWeather();
    }
}

function getNewWeather(){
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 5000, maximumAge: 0}
    );
}

function sendToApp(conditions){
    var dictionary = {
        "KEY_CONDITIONS": conditions
    };

    Pebble.sendAppMessage(dictionary,
                          function () {
                              console.log("sent: " + JSON.stringify(dictionary));
                          },
                          function () {
                              console.log("error in sending: " + JSON.stringify(dictionary));
                          }
                         );
}

Pebble.addEventListener('ready', getWeather);

Pebble.addEventListener('appmessage', getWeather);