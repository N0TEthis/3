#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <clocale>
#include <stdexcept>
using namespace std;

class Date {
private:
    int day;
    int month;
    int year;

    // високосный ли год
    static bool isLeap(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    // кол-во дней в месяце m года y
    static int daysInMonth(int m, int y) {
        if (m == 4 || m == 6 || m == 9 || m == 11)
            return 30;
        if (m == 2) {
            if (isLeap(y)) return 29;
            return 28;
        }
        return 31;
    }

    // проверочка существования даты
    static bool isValid(int d, int m, int y) {
        if (y < 1) return false;
        if (m < 1 || m > 12) return false;
        return d >= 1 && d <= daysInMonth(m, y);
    }

    // порядковый номер дня: 01.01.0001 -> 1
    long long toDays() const {
        long long y = year - 1;
        long long n = 365 * y + y / 4 - y / 100 + y / 400;
        for (int i = 1; i < month; i++)
            n += daysInMonth(i, year);
        return n + day;
    }

    // обратное преобразование: номер дня --> дата
    static Date fromDays(long long n) {
        if (n < 1)
            throw out_of_range("Дата раньше 01.01.0001");
        int y = 1;
        while (n > (isLeap(y) ? 366 : 365)) {   // отбрасываем целые годы
            n -= isLeap(y) ? 366 : 365;
            y++;
        }
        int m = 1;
        while (n > daysInMonth(m, y)) {         // отбрасываем целые месяцы
            n -= daysInMonth(m, y);
            m++;
        }
        return Date((int)n, m, y);              // остаток - день месяца
    }

public:
    // конструктор с проверкой даты
    Date(int d = 1, int m = 1, int y = 2000) {
        if (!isValid(d, m, y))
            throw invalid_argument("Такой даты не существует");
        day = d;
        month = m;
        year = y;
    }

    int getDay() const   { return day; }
    int getMonth() const { return month; }
    int getYear() const  { return year; }

    // разность в днях: this - other
    // (положительная, если this позже other)
    long long diff(const Date& other) const {
        return toDays() - other.toDays();
    }

    // дата, отстоящая на n дней (n может быть отрицательным)
    Date addDays(int n) const {
        return fromDays(toDays() + n);
    }

    // дата, отстоящая на k месяцев (k может быть отрицательным)
    Date addMonths(int k) const {
        int total = (month - 1) + k;
        int newYear = year + total / 12;
        int newMonth = total % 12;
        if (newMonth < 0) {         // C++ даёт отрицательный остаток
            newMonth += 12;
            newYear--;
        }
        newMonth++;                 // обратно в диапазон 1..12

        int newDay = day;
        if (newYear >= 1) {         // 31 января + 1 месяц -> 28/29 февраля
            int maxDay = daysInMonth(newMonth, newYear);
            if (newDay > maxDay) newDay = maxDay;
        }
        return Date(newDay, newMonth, newYear);
    }

    // День недели: 0 - понедельник, ..., 6 - воскресенье
    int dayOfWeek() const {
        return (int)((toDays() - 1) % 7);
    }

    string dayName() const {
        const string names[7] = {
            "Понедельник",
            "Вторник",
            "Среда",
            "Четверг",
            "Пятница",
            "Суббота",
            "Воскресенье"
        };
        return names[dayOfWeek()];
    }

    void print() const {
        cout << setfill('0') << setw(2) << day << '.'
             << setw(2) << month << '.'
             << setw(4) << year << setfill(' ');
    }
};

// Ввод даты с повтором при ошибке
Date readDate(const string& text) {
    while (true) {
        int d, m, y;
        cout << text << " (день месяц год): ";
        cin >> d >> m >> y;
        if (!cin) {                         // введены не числа
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка ввода. Повторите.\n";
            continue;
        }
        try {
            return Date(d, m, y);
        }
        catch (const invalid_argument& e) {
            cout << "Ошибка: " << e.what() << ". Повторите ввод.\n";
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    Date d1 = readDate("Введите дату D1");
    int choice;
    do {
        cout << "\nD1 = ";
        d1.print();
        cout << "\n1 - разность двух дат в днях\n"
             << "2 - дата через n дней\n"
             << "3 - дата через k месяцев\n"
             << "4 - день недели\n"
             << "0 - выход\n"
             << "Ваш выбор: ";
        cin >> choice;

        try {
            if (choice == 1) {
                Date d2 = readDate("Введите дату D2");
                cout << "Между датами " << llabs(d1.diff(d2)) << " дн.\n";
            }
            else if (choice == 2) {
                int n;
                cout << "Сколько дней прибавить (можно отрицательное): ";
                cin >> n;
                Date r = d1.addDays(n);
                cout << "Результат: ";
                r.print();
                cout << "\n";
            }
            else if (choice == 3) {
                int k;
                cout << "Сколько месяцев прибавить (можно отрицательное): ";
                cin >> k;
                Date r = d1.addMonths(k);
                cout << "Результат: ";
                r.print();
                cout << "\n";
            }
            else if (choice == 4) {
                cout << "День недели: " << d1.dayName() << "\n";
            }
            else if (choice == 0) {
                cout << "До встречи !\n";
            }
            else {
                cout << "Нет такого пункта меню.\n";
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
        }
    } while (choice != 0);

    return 0;
}