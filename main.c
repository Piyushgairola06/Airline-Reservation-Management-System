#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESERVATIONS 100
#define MAX_SEATS 150
#define BASE_TICKET_PRICE 150.0
#define SEASONAL_INCREMENT 0.3  // 30% increase during peak season
#define CANCELLATION_FEE 0.2    // 20% deduction on cancellation
#define MAX_FLIGHTS 5           // Number of predefined flights

// Flight structure
typedef struct {
    char flightNumber[10];
    char destination[50];
    char departureTime[10];
} Flight;

// Date structure
typedef struct {
    int day;
    int month;
    int year;
} Date;

// Reservation structure 
typedef struct {
    char name[50];
    char flightNumber[10];
    int seatNumber;
    char pnr[10];
    double ticketPrice;
    Date bookingDate;
} Reservation;

// Global variables
Flight flights[MAX_FLIGHTS] = {
    {"FL123", "New York", "08:00"},
    {"FL456", "London", "10:30"},
    {"FL789", "Tokyo", "14:00"},
    {"FL101", "Sydney", "16:45"},
    {"FL202", "Paris", "19:15"}
};

Reservation reservations[MAX_RESERVATIONS];
int reservationCount = 0;
static int pnrCounter = 1000;

// User DEfined Functions
void bookTicket();
void cancelTicket();
void modifyReservation();
void displayPassengerList();
void saveToFile();
void loadFromFile();
void generatePNR(char *pnr);
void checkSeatAvailability();
int isSeatAvailable(char *flight, int seat);
double calculateTicketPrice(int isPeak);
void displayFlights();
int isValidFlight(char *flight);
int isValidDate(int day, int month, int year);
int isPeakSeason(Date date);
void parseDate(const char *dateStr, Date *date);

int main() {
    int choice;

    loadFromFile();

    while (1) {
        printf("\n========================================\n");
        printf("         Airline Reservation System      \n");
        printf("========================================\n");
        displayFlights();
        printf("\n1. Book a Ticket\n");
        printf("2. Cancel a Ticket\n");
        printf("3. Modify Reservation\n");
        printf("4. Check Seat Availability\n");
        printf("5. Display Passenger List\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                bookTicket();
                break;
            case 2:
                cancelTicket();
                break;
            case 3:
                modifyReservation();
                break;
            case 4:
                checkSeatAvailability();
                break;
            case 5:
                displayPassengerList();
                break;
            case 6:
                printf("Thank you for using the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

void displayFlights() {
    printf("Available Flights:\n");
    printf("+----------+-----------------+----------------+\n");
    printf("| Flight No| Destination     | Departure Time |\n");
    printf("+----------+-----------------+----------------+\n");
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        printf("| %-8s | %-15s | %-14s |\n", 
               flights[i].flightNumber, 
               flights[i].destination, 
               flights[i].departureTime);
    }
    printf("+----------+-----------------+----------------+\n");
}

int isValidFlight(char *flight) {
    for (int i = 0; i < MAX_FLIGHTS; i++) {
        if (strcmp(flights[i].flightNumber, flight) == 0) {
            return 1;
        }
    }
    return 0;
}

void generatePNR(char *pnr) {
    sprintf(pnr, "PNR%d", pnrCounter++);
}

int isSeatAvailable(char *flight, int seat) {
    for (int i = 0; i < reservationCount; i++) {
        if (strcmp(reservations[i].flightNumber, flight) == 0 &&
            reservations[i].seatNumber == seat) {
            return 0;
        }
    }
    return 1;
}

double calculateTicketPrice(int isPeak) {
    double price = BASE_TICKET_PRICE;
    if (isPeak) {
        price *= (1.0 + SEASONAL_INCREMENT);
    }
    return price;
}

int isValidDate(int day, int month, int year) {
    if (year < 2025 || year > 2030) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1) return 0;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[1] = 29; // Leap year
    }
    return day <= daysInMonth[month - 1];
}

void parseDate(const char *dateStr, Date *date) {
    sscanf(dateStr, "%d/%d/%d", &date->day, &date->month, &date->year);
}

int isPeakSeason(Date date) {
    return (date.month == 6 || date.month == 7 || date.month == 8 || date.month == 12);
}

void bookTicket() {
    if (reservationCount >= MAX_RESERVATIONS) {
        printf("Sorry, all reservations are full.\n");
        return;
    }

    char name[50], flight[10], dateStr[11];
    int seat;
    Date bookingDate;

    printf("Enter passenger name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter flight number (from available flights above): ");
    fgets(flight, 10, stdin);
    flight[strcspn(flight, "\n")] = 0;

    if (!isValidFlight(flight)) {
        printf("Invalid flight number. Please choose from the available flights.\n");
        return;
    }

    printf("Enter desired seat number (1 to %d): ", MAX_SEATS);
    scanf("%d", &seat);
    getchar();

    if (seat < 1 || seat > MAX_SEATS) {
        printf("Invalid seat number. Please choose between 1 and %d.\n", MAX_SEATS);
        return;
    }

    if (!isSeatAvailable(flight, seat)) {
        printf("Sorry, seat %d is already booked on flight %s.\n", seat, flight);
        return;
    }

    printf("Enter booking date (DD/MM/YYYY): ");
    fgets(dateStr, 11, stdin);
    dateStr[strcspn(dateStr, "\n")] = 0;
    parseDate(dateStr, &bookingDate);

    if (!isValidDate(bookingDate.day, bookingDate.month, bookingDate.year)) {
        printf("Invalid date. Booking cancelled.\n");
        return;
    }

    strcpy(reservations[reservationCount].name, name);
    strcpy(reservations[reservationCount].flightNumber, flight);
    reservations[reservationCount].seatNumber = seat;
    reservations[reservationCount].bookingDate = bookingDate;
    reservations[reservationCount].ticketPrice = calculateTicketPrice(isPeakSeason(bookingDate));
    generatePNR(reservations[reservationCount].pnr);

    printf("Booking successful! Your PNR is: %s\n", reservations[reservationCount].pnr);
    printf("Ticket Price: $%.2f\n", reservations[reservationCount].ticketPrice);
    if (isPeakSeason(bookingDate)) {
        printf("Note: Peak season pricing applied (%.0f%% increase)\n", SEASONAL_INCREMENT * 100);
    }

    reservationCount++;
    saveToFile();
}

void cancelTicket() {
    char pnr[10];
    printf("Enter your PNR to Cancel the reservation: ");
    fgets(pnr, 10, stdin);
    pnr[strcspn(pnr, "\n")] = 0;

    for (int i = 0; i < reservationCount; i++) {
        if (strcmp(reservations[i].pnr, pnr) == 0) {
            double refundAmount = reservations[i].ticketPrice * (1.0 - CANCELLATION_FEE);
            printf("Reservation found:\n");
            printf("Passenger: %s\n", reservations[i].name);
            printf("Flight: %s, Seat: %d\n", reservations[i].flightNumber, reservations[i].seatNumber);
            printf("Booking Date: %02d/%02d/%04d\n", 
                   reservations[i].bookingDate.day, 
                   reservations[i].bookingDate.month, 
                   reservations[i].bookingDate.year);
            printf("Original Price: $%.2f\n", reservations[i].ticketPrice);
            printf("Refund Amount (after %.0f%% cancellation fee): $%.2f\n", 
                   CANCELLATION_FEE * 100, refundAmount);

            for (int j = i; j < reservationCount - 1; j++) {
                reservations[j] = reservations[j + 1];
            }
            reservationCount--;
            saveToFile();
            printf("Your reservation has been canceled successfully.\n");
            return;
        }
    }

    printf("PNR not found. Please check and try again.\n");
}

void modifyReservation() {
    char pnr[10];
    printf("Enter your PNR to modify the reservation: ");
    fgets(pnr, 10, stdin);
    pnr[strcspn(pnr, "\n")] = 0;

    for (int i = 0; i < reservationCount; i++) {
        if (strcmp(reservations[i].pnr, pnr) == 0) {
            printf("Current Reservation Details:\n");
            printf("Passenger: %s\n", reservations[i].name);
            printf("Flight: %s\n", reservations[i].flightNumber);
            printf("Seat: %d\n", reservations[i].seatNumber);
            printf("Booking Date: %02d/%02d/%04d\n", 
                   reservations[i].bookingDate.day, 
                   reservations[i].bookingDate.month, 
                   reservations[i].bookingDate.year);
            printf("Price: $%.2f\n", reservations[i].ticketPrice);

            printf("\nEnter new passenger name (or press Enter to keep current): ");
            char name[50];
            fgets(name, 50, stdin);
            name[strcspn(name, "\n")] = 0;
            if (strlen(name) > 0) {
                strcpy(reservations[i].name, name);
            }

            printf("Enter new flight number (or press Enter to keep current, from available flights):\n");
            displayFlights();
            char flight[10];
            fgets(flight, 10, stdin);
            flight[strcspn(flight, "\n")] = 0;
            if (strlen(flight) > 0) {
                if (isValidFlight(flight)) {
                    strcpy(reservations[i].flightNumber, flight);
                } else {
                    printf("Invalid flight number. Keeping original flight.\n");
                }
            }

            printf("Enter new seat number (1 to %d, or 0 to keep current): ", MAX_SEATS);
            int seat;
            scanf("%d", &seat);
            getchar();

            if (seat >= 1 && seat <= MAX_SEATS) {
                if (isSeatAvailable(reservations[i].flightNumber, seat)) {
                    reservations[i].seatNumber = seat;
                } else {
                    printf("Seat %d is not available. Keeping original seat.\n", seat);
                }
            }

            printf("Enter new booking date (DD/MM/YYYY, or press Enter to keep current): ");
            char dateStr[11];
            fgets(dateStr, 11, stdin);
            dateStr[strcspn(dateStr, "\n")] = 0;
            if (strlen(dateStr) > 0) {
                Date newDate;
                parseDate(dateStr, &newDate);
                if (isValidDate(newDate.day, newDate.month, newDate.year)) {
                    reservations[i].bookingDate = newDate;
                    reservations[i].ticketPrice = calculateTicketPrice(isPeakSeason(newDate));
                } else {
                    printf("Invalid date. Keeping original booking date.\n");
                }
            } else {
                reservations[i].ticketPrice = calculateTicketPrice(isPeakSeason(reservations[i].bookingDate));
            }

            saveToFile();
            printf("Reservation modified successfully!\n");
            printf("Updated Price: $%.2f\n", reservations[i].ticketPrice);
            if (isPeakSeason(reservations[i].bookingDate)) {
                printf("Note: Peak season pricing applied (%.0f%% increase)\n", SEASONAL_INCREMENT * 100);
            }
            return;
        }
    }

    printf("PNR not found. Please check and try again.\n");
}

void checkSeatAvailability() {
    char flight[10];
    printf("Enter flight number to check seat availability (from available flights):\n");
    displayFlights();
    fgets(flight, 10, stdin);
    flight[strcspn(flight, "\n")] = 0;

    if (!isValidFlight(flight)) {
        printf("Invalid flight number. Please choose from the available flights.\n");
        return;
    }

    int booked[MAX_SEATS + 1] = {0};

    for (int i = 0; i < reservationCount; i++) {
        if (strcmp(reservations[i].flightNumber, flight) == 0) {
            booked[reservations[i].seatNumber] = 1;
        }
    }

    printf("Seat availability for flight %s (X = reserved):\n", flight);
    int count = 0;
    for (int i = 1; i <= MAX_SEATS; i++) {
        if (!booked[i]) {
            printf("%3d ", i);
            count++;
        } else {
            printf("  X ");
        }
        if (i % 10 == 0) {
            printf("\n");
        }
    }

    printf("\nTotal available seats: %d\n", count);
    printf("Current ticket price: $%.2f\n", calculateTicketPrice(0)); // Assume non-peak for display
}

void displayPassengerList() {
    char flight[10];
    printf("Enter flight number to display passenger list (from available flights):\n");
    displayFlights();
    fgets(flight, 10, stdin);
    flight[strcspn(flight, "\n")] = 0;

    if (!isValidFlight(flight)) {
        printf("Invalid flight number. Please choose from the available flights.\n");
        return;
    }

    int found = 0;
    printf("\nPassenger List for Flight %s\n", flight);
    printf("+--------------------------------+------------+--------+-------------+---------------+\n");
    printf("| Passenger Name                 | Seat Number| PNR    | Ticket Price| Booking Date  |\n");
    printf("+--------------------------------+------------+--------+-------------+---------------+\n");

    for (int i = 0; i < reservationCount; i++) {
        if (strcmp(reservations[i].flightNumber, flight) == 0) {
            found = 1;
            printf("| %-30s | %10d | %-6s | $%10.2f | %02d/%02d/%04d |\n", 
                   reservations[i].name, 
                   reservations[i].seatNumber, 
                   reservations[i].pnr, 
                   reservations[i].ticketPrice,
                   reservations[i].bookingDate.day,
                   reservations[i].bookingDate.month,
                   reservations[i].bookingDate.year);
        }
    }

    printf("+--------------------------------+------------+--------+-------------+---------------+\n");

    if (!found) {
        printf("No passengers found for flight %s.\n", flight);
    }
}

void saveToFile() {
    FILE *file = fopen("reservations.txt", "w");
    if (!file) {
        printf("Error saving to file.\n");
        return;
    }

    for (int i = 0; i < reservationCount; i++) {
        fprintf(file, "\"%s\" %s %d %s %.2f %02d/%02d/%04d\n",
                reservations[i].name,
                reservations[i].flightNumber,
                reservations[i].seatNumber,
                reservations[i].pnr,
                reservations[i].ticketPrice,
                reservations[i].bookingDate.day,
                reservations[i].bookingDate.month,
                reservations[i].bookingDate.year);
    }

    fclose(file);
}

void loadFromFile() {
    FILE *file = fopen("reservations.txt", "r");
    if (!file) {
        return;
    }

    char name[50];
    char flightNumber[10];
    int seatNumber;
    char pnr[10];
    double ticketPrice;
    int day, month, year;

    while (fscanf(file, "\"%49[^\"]\" %9s %d %9s %lf %d/%d/%d",
                  name, flightNumber, &seatNumber, pnr, &ticketPrice,
                  &day, &month, &year) == 8) {
        if (reservationCount < MAX_RESERVATIONS) {
            strcpy(reservations[reservationCount].name, name);
            strcpy(reservations[reservationCount].flightNumber, flightNumber);
            reservations[reservationCount].seatNumber = seatNumber;
            strcpy(reservations[reservationCount].pnr, pnr);
            reservations[reservationCount].ticketPrice = ticketPrice;
            reservations[reservationCount].bookingDate.day = day;
            reservations[reservationCount].bookingDate.month = month;
            reservations[reservationCount].bookingDate.year = year;
            reservationCount++;
        }
    }

    fclose(file);
}