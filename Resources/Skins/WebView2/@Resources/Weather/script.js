// Weather Widget Script
const API_KEY = 'd87d34d2cddf097a3710c1627a0d024d'; // Users can get free API key from openweathermap.org
const USE_GEOLOCATION = false; // Set to true to use browser geolocation (requires windows location to be enabled)

// Manual coordinates - UPDATE THESE TO YOUR LOCATION
const MANUAL_LAT = 29.712885630493965; 
const MANUAL_LON = 72.4520053233683;

// DOM Elements
const loadingEl = document.getElementById('loading');
const errorEl = document.getElementById('error');
const errorMessageEl = document.getElementById('error-message');
const weatherContentEl = document.getElementById('weather-content');
const cityEl = document.getElementById('city');
const tempEl = document.getElementById('temp');
const descriptionEl = document.getElementById('description');
const weatherIconEl = document.getElementById('weather-icon');
const humidityEl = document.getElementById('humidity');
const windEl = document.getElementById('wind');
const feelsLikeEl = document.getElementById('feels-like');

// Weather icon mapping
const weatherIcons = {
    '01d': 'ph-sun',           // clear sky day
    '01n': 'ph-moon',          // clear sky night
    '02d': 'ph-cloud-sun',     // few clouds day
    '02n': 'ph-cloud-moon',    // few clouds night
    '03d': 'ph-cloud',         // scattered clouds
    '03n': 'ph-cloud',
    '04d': 'ph-clouds',        // broken clouds
    '04n': 'ph-clouds',
    '09d': 'ph-cloud-rain',    // shower rain
    '09n': 'ph-cloud-rain',
    '10d': 'ph-cloud-rain',    // rain day
    '10n': 'ph-cloud-rain',    // rain night
    '11d': 'ph-lightning',     // thunderstorm
    '11n': 'ph-lightning',
    '13d': 'ph-snowflake',     // snow
    '13n': 'ph-snowflake',
    '50d': 'ph-wind',          // mist
    '50n': 'ph-wind'
};

// Show error
function showError(message) {
    if (loadingEl) loadingEl.classList.add('hidden');
    if (weatherContentEl) weatherContentEl.classList.add('hidden');
    if (errorMessageEl) errorMessageEl.textContent = message;
    if (errorEl) errorEl.classList.remove('hidden');
}

// Show weather
function showWeather() {
    if (loadingEl) loadingEl.classList.add('hidden');
    if (errorEl) errorEl.classList.add('hidden');
    if (weatherContentEl) weatherContentEl.classList.remove('hidden');
}

// Update weather icon
function updateWeatherIcon(iconCode) {
    const iconClass = weatherIcons[iconCode] || 'ph-sun';
    if (weatherIconEl) {
        weatherIconEl.innerHTML = `<i class="ph ${iconClass}"></i>`;
    }
}

// Fetch weather data
async function fetchWeather(lat, lon) {
    try {
        const url = `https://api.openweathermap.org/data/2.5/weather?lat=${lat}&lon=${lon}&units=metric&appid=${API_KEY}`;
        
        const response = await fetch(url, {
            method: 'GET',
            mode: 'cors',
            cache: 'no-cache',
            credentials: 'omit' // Don't send credentials to avoid tracking prevention
        });
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        
        // Update UI with null checks
        if (cityEl) cityEl.textContent = `${data.name}, ${data.sys.country}`;
        if (tempEl) tempEl.textContent = Math.round(data.main.temp);
        if (descriptionEl) descriptionEl.textContent = data.weather[0].description;
        if (humidityEl) humidityEl.textContent = `${data.main.humidity}%`;
        if (windEl) windEl.textContent = `${Math.round(data.wind.speed * 3.6)} km/h`;
        if (feelsLikeEl) feelsLikeEl.textContent = `${Math.round(data.main.feels_like)}°`;
        
        updateWeatherIcon(data.weather[0].icon);
        showWeather();
        
    } catch (error) {
        console.error('Error fetching weather:', error);
        showError(`Error: ${error.message}`);
    }
}

// Get user location
function getUserLocation() {
    if (USE_GEOLOCATION && navigator.geolocation) {
        // Try geolocation with timeout
        const options = {
            enableHighAccuracy: false,
            timeout: 5000,
            maximumAge: 0
        };
        
        navigator.geolocation.getCurrentPosition(
            (position) => {
                const lat = position.coords.latitude;
                const lon = position.coords.longitude;
                fetchWeather(lat, lon);
            },
            (error) => {
                console.warn('Geolocation failed, using manual coordinates:', error.message);
                // Fallback to manual coordinates
                fetchWeather(MANUAL_LAT, MANUAL_LON);
            },
            options
        );
    } else {
        // Use manual coordinates
        fetchWeather(MANUAL_LAT, MANUAL_LON);
    }
}

// Initialize
function init() {
    try {
        // Check if API key is set
        if (API_KEY === 'YOUR_API_KEY') {
            showError('Please set your API key in script.js');
            return;
        }
        
        // Initial fetch
        getUserLocation();
        
        // Refresh weather every 10 minutes
        setInterval(getUserLocation, 600000);
        
    } catch (error) {
        console.error('Initialization error:', error);
        showError('Failed to initialize widget');
    }
}

// Wait for DOM to be ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}
