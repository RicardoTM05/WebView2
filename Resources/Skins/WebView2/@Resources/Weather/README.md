# Weather Widget Setup

## Getting Your API Key

1. Go to [OpenWeatherMap](https://openweathermap.org/api)
2. Sign up for a free account
3. Navigate to "API keys" section
4. Copy your API key

## Configuration

Open `script.js` and update the following:

```javascript
const API_KEY = 'YOUR_API_KEY'; // Replace with your actual API key
const USE_GEOLOCATION = true; // Set to false to use manual coordinates

// If using manual coordinates, update these:
const MANUAL_LAT = 31.5204; // Your latitude
const MANUAL_LON = 74.3587; // Your longitude
```

## Features

- **Auto-location detection**: Uses browser geolocation API
- **Manual coordinates**: Fallback option if geolocation fails
- **Real-time weather**: Temperature, humidity, wind speed, feels like
- **Weather icons**: Dynamic icons based on current conditions
- **Auto-refresh**: Updates every 10 minutes
- **Beautiful UI**: Glassmorphic design with animated background

## Weather Icons

The widget displays different icons based on weather conditions:
- ☀️ Clear sky
- ☁️ Clouds
- 🌧️ Rain
- ⛈️ Thunderstorm
- ❄️ Snow
- 🌫️ Mist/Fog

## Troubleshooting

**"Please set your OpenWeatherMap API key"**
- You need to replace `YOUR_API_KEY` with your actual API key

**"Unable to fetch weather data"**
- Check your internet connection
- Verify your API key is correct
- Make sure your API key is activated (can take a few minutes)

**Location not accurate**
- Enable browser location permissions
- Or set manual coordinates in `script.js`
