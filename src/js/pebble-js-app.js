var CONDITION_KEY = {
    clear_day: 0,
    clear_night: 1,
    cloudy: 2,
    rain: 3,
    snow: 4,
    thunder: 5,
    fog: 6,
    blank: 7,
    unknown: 8
}

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

            var iconKey = json.weather[0].icon;
            var conditions;
            switch(iconKey){
                case '01d':
                case '01n':
                    conditions = CONDITION_KEY.clear_day;
                    conditions = CONDITION_KEY.clear_night;
                    break;
                case '03d':
                case '03n':
                case '04d':
                case '04n':
                    conditions = CONDITION_KEY.cloudy;
                    break;
                case '09d':
                case '09n':
                case '10d':
                case '10n':
                    conditions = CONDITION_KEY.rain;
                    break;
                case '11d':
                case '11n':
                    conditions = CONDITION_KEY.thunder;
                    break;
                case '13d':
                case '13n':
                    conditions = CONDITION_KEY.snow;
                    break;
                case '50d':
                case '50n':
                    conditions = CONDITION_KEY.fog;
                    break;
                default:
                    conditions = CONDITION_KEY.unknown;
                    break;
            }

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
    );
}

function locationError(err) {
    console.log("could not get location");
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 5000, maximumAge: 0}
    );
}

Pebble.addEventListener('ready', getWeather);

Pebble.addEventListener('appmessage', getWeather);