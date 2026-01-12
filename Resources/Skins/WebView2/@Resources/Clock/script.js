// ---- Helpers ----
const loadVariable = (key, defaultValue) => {
	const value = RainmeterAPI.GetVariable(String(key));
	if (value.includes("#")) {
		writeVariable(String(key), String(defaultValue));
		return String(defaultValue);
	}
	return value;
};

const writeVariable = (key, value) => {
	RainmeterAPI.Bang(`[!WriteKeyValue Variables "${key}" "${value}"]`);
};

// ---- Variables ----
// We use Intl.DateTimeFormat().resolvedOptions to get the user's locale and timezone.
const userLocale = Intl.DateTimeFormat().resolvedOptions().locale;
const userTimeZone = Intl.DateTimeFormat().resolvedOptions().timeZone;

let timeZone = loadVariable('timezone', userTimeZone);
let locale = loadVariable('locale', userLocale);
let format = loadVariable('format', '12h');

// ---- DOM cache ----
const dom = {
	hours: document.getElementById('hours'),
	minutes: document.getElementById('minutes'),
	seconds: document.getElementById('seconds'),
	ampm: document.getElementById('ampm'),
	date: document.getElementById('date')
};

// ---- Clock logic ----
function toggleLocale() {
	locale = locale === userLocale ? 'en-US' : userLocale;
	writeVariable('locale', locale);
	updateClock();
}

function toggleFormat() {
	format = format === '12h' ? '24h' : '12h';
	writeVariable('format', format);
	updateClock();
}

function updateClock() {
	const now = new Date();

	const timeFormatter = new Intl.DateTimeFormat(locale, {
		timeZone,
		hour: '2-digit',
		minute: '2-digit',
		second: '2-digit',
		hour12: format === '12h'
	});

	const dateFormatter = new Intl.DateTimeFormat(locale, {
		timeZone,
		weekday: 'long',
		month: 'long',
		day: 'numeric'
	});

	const timeParts = timeFormatter.formatToParts(now);
	const dateParts = dateFormatter.formatToParts(now);

	const get = (parts, type) =>
		parts.find(p => p.type === type)?.value || '';

	dom.hours.textContent = get(timeParts, 'hour');
	dom.minutes.textContent = get(timeParts, 'minute');
	dom.seconds.textContent = get(timeParts, 'second');
	dom.ampm.textContent = get(timeParts, 'dayPeriod');

	dom.date.textContent = dateParts.map(p => p.value).join('');
}

// ---- Start clock ----
updateClock();
// Update clock once a second.
setInterval(updateClock, 1000); 
