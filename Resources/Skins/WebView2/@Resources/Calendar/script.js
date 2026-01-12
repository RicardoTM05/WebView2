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

const date = new Date();
const userLocale = Intl.DateTimeFormat().resolvedOptions().locale;

let locale = loadVariable('locale', userLocale);

function toggleLocale() {
	locale = locale === userLocale ? 'en-US' : userLocale;
	writeVariable('locale', locale);
	renderWeekdays();
	renderCalendar();
}

const renderWeekdays = () => {
	const weekdaysContainer = document.querySelector(".weekdays");
	weekdaysContainer.innerHTML = "";

	const baseDate = new Date(Date.UTC(2026, 0, 5)); // Sunday

	const formatter = new Intl.DateTimeFormat(locale, {
		weekday: 'short'
	});

	for (let i = 0; i < 7; i++) {
		const day = new Date(baseDate);
		day.setUTCDate(baseDate.getUTCDate() + i);

		const name = formatter.format(day);
		weekdaysContainer.innerHTML += `<span>${name}</span>`;
	}
};

const renderCalendar = () => {
	date.setDate(1);

	const monthDays = document.querySelector(".days");

	const lastDay = new Date(
		date.getFullYear(),
		date.getMonth() + 1,
		0
	).getDate();

	const prevLastDay = new Date(
		date.getFullYear(),
		date.getMonth(),
		0
	).getDate();

	const firstDayIndex =
		(new Intl.DateTimeFormat(locale, { weekday: 'short' })
			.formatToParts(date)
			.find(p => p.type === 'weekday') ? date.getDay() : date.getDay());

	const lastDayIndex = new Date(
		date.getFullYear(),
		date.getMonth() + 1,
		0
	).getDay();

	const nextDays = 7 - lastDayIndex - 1;

	// Localized month name
	const monthFormatter = new Intl.DateTimeFormat(locale, { month: 'long' });

	document.querySelector("#current-month").textContent =
		monthFormatter.format(date);

	document.querySelector("#current-year").textContent =
		date.getFullYear();

	let days = "";

	// Previous month's days
	for (let x = firstDayIndex; x > 0; x--) {
		days += `<div class="prev-date">${prevLastDay - x + 1}</div>`;
	}

	// Current month's days
	const today = new Date();

	for (let i = 1; i <= lastDay; i++) {
		if (
			i === today.getDate() &&
			date.getMonth() === today.getMonth() &&
			date.getFullYear() === today.getFullYear()
		) {
			days += `<div class="today">${i}</div>`;
		} else {
			days += `<div>${i}</div>`;
		}
	}

	// Next month's days
	for (let j = 1; j <= nextDays; j++) {
		days += `<div class="next-date">${j}</div>`;
	}

	monthDays.innerHTML = days;
};

document.querySelector("#prev-month").addEventListener("click", () => {
    date.setMonth(date.getMonth() - 1);
    renderCalendar();
});

document.querySelector("#next-month").addEventListener("click", () => {
    date.setMonth(date.getMonth() + 1);
    renderCalendar();
});

renderWeekdays();
renderCalendar();
