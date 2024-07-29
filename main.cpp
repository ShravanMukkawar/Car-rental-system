#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// Abstraction: Define an interface for the basic Car operations
class Rentable {
public:
    virtual double calculatePrice(int rentalDays) = 0;
    virtual bool isAvailable() = 0;
    virtual void rent() = 0;
    virtual void returnCar() = 0;
    virtual string getCarType() = 0;
};

// Encapsulation: Base class Car
class Car : public Rentable {
private:
    string carId;
    string brand;
    string model;
    double basePricePerDay;
    bool available;

public:
    Car(string carId, string brand, string model, double basePricePerDay) 
        : carId(carId), brand(brand), model(model), basePricePerDay(basePricePerDay), available(true) {}

    string getCarId() const { return carId; }
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    double getBasePricePerDay() const { return basePricePerDay; }

    double calculatePrice(int rentalDays) override {
        return basePricePerDay * rentalDays;
    }

    bool isAvailable() override {
        return available;
    }

    void rent() override {
        available = false;
    }

    void returnCar() override {
        available = true;
    }
};

// Inheritance: Derived class EconomyCar
class EconomyCar : public Car {
public:
    EconomyCar(string carId, string brand, string model, double basePricePerDay)
        : Car(carId, brand, model, basePricePerDay) {}

    string getCarType() override {
        return "Economy";
    }
};

// Inheritance: Derived class LuxuryCar
class LuxuryCar : public Car {
public:
    LuxuryCar(string carId, string brand, string model, double basePricePerDay)
        : Car(carId, brand, model, basePricePerDay) {}

    string getCarType() override {
        return "Luxury";
    }

    double calculatePrice(int rentalDays) override {
        // Luxury cars have a 20% surcharge
        return Car::calculatePrice(rentalDays) * 1.2;
    }
};

// Customer class
class Customer {
private:
    string customerId;
    string name;

public:
    Customer(string customerId, string name) : customerId(customerId), name(name) {}

    string getCustomerId() const { return customerId; }
    string getName() const { return name; }
};

// Rental class
class Rental {
private:
    shared_ptr<Car> car;
    shared_ptr<Customer> customer;
    int days;

public:
    Rental(shared_ptr<Car> car, shared_ptr<Customer> customer, int days) 
        : car(car), customer(customer), days(days) {}

    shared_ptr<Car> getCar() const { return car; }
    shared_ptr<Customer> getCustomer() const { return customer; }
    int getDays() const { return days; }
};

// CarRentalSystem class
class CarRentalSystem {
private:
    vector<shared_ptr<Car>> cars;
    vector<shared_ptr<Customer>> customers;
    vector<Rental> rentals;

public:
    void addCar(shared_ptr<Car> car) {
        cars.push_back(car);
    }

    void addCustomer(shared_ptr<Customer> customer) {
        customers.push_back(customer);
    }

    void rentCar(shared_ptr<Car> car, shared_ptr<Customer> customer, int days) {
        if (car->isAvailable()) {
            car->rent();
            rentals.push_back(Rental(car, customer, days));
        } else {
            cout << "Car is not available for rent." << endl;
        }
    }

    void returnCar(shared_ptr<Car> car) {
        car->returnCar();
        for (auto it = rentals.begin(); it != rentals.end(); ++it) {
            if (it->getCar() == car) {
                rentals.erase(it);
                return;
            }
        }
        cout << "Car was not rented." << endl;
    }

    void menu() {
        while (true) {
            cout << "===== Car Rental System =====" << endl;
            cout << "1. Rent a Car" << endl;
            cout << "2. Return a Car" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore(); // Consume newline

            if (choice == 1) {
                cout << "\n== Rent a Car ==\n" << endl;
                cout << "Enter your name: ";
                string customerName;
                getline(cin, customerName);

                cout << "\nAvailable Cars:" << endl;
                for (const auto& car : cars) {
                    if (car->isAvailable()) {
                        cout << car->getCarId() << " - " << car->getBrand() << " " << car->getModel() 
                             << " (" << car->getCarType() << ")" << endl;
                    }
                }

                cout << "\nEnter the car ID you want to rent: ";
                string carId;
                getline(cin, carId);

                cout << "Enter the number of days for rental: ";
                int rentalDays;
                cin >> rentalDays;
                cin.ignore(); // Consume newline

                auto newCustomer = make_shared<Customer>("CUS" + to_string(customers.size() + 1), customerName);
                addCustomer(newCustomer);

                shared_ptr<Car> selectedCar = nullptr;
                for (const auto& car : cars) {
                    if (car->getCarId() == carId && car->isAvailable()) {
                        selectedCar = car;
                        break;
                    }
                }

                if (selectedCar) {
                    double totalPrice = selectedCar->calculatePrice(rentalDays);
                    cout << "\n== Rental Information ==\n" << endl;
                    cout << "Customer ID: " << newCustomer->getCustomerId() << endl;
                    cout << "Customer Name: " << newCustomer->getName() << endl;
                    cout << "Car: " << selectedCar->getBrand() << " " << selectedCar->getModel() 
                         << " (" << selectedCar->getCarType() << ")" << endl;
                    cout << "Rental Days: " << rentalDays << endl;
                    printf("Total Price: $%.2f\n", totalPrice);

                    cout << "\nConfirm rental (Y/N): ";
                    string confirm;
                    getline(cin, confirm);

                    if (confirm == "Y" || confirm == "y") {
                        rentCar(selectedCar, newCustomer, rentalDays);
                        cout << "\nCar rented successfully." << endl;
                    } else {
                        cout << "\nRental canceled." << endl;
                    }
                } else {
                    cout << "\nInvalid car selection or car not available for rent." << endl;
                }
            } else if (choice == 2) {
                cout << "\n== Return a Car ==\n" << endl;
                cout << "Enter the car ID you want to return: ";
                string carId;
                getline(cin, carId);

                shared_ptr<Car> carToReturn = nullptr;
                for (const auto& car : cars) {
                    if (car->getCarId() == carId && !car->isAvailable()) {
                        carToReturn = car;
                        break;
                    }
                }

                if (carToReturn) {
                    shared_ptr<Customer> customer;
                    for (const auto& rental : rentals) {
                        if (rental.getCar() == carToReturn) {
                            customer = rental.getCustomer();
                            break;
                        }
                    }

                    if (customer) {
                        returnCar(carToReturn);
                        cout << "Car returned successfully by " << customer->getName() << endl;
                    } else {
                        cout << "Car was not rented or rental information is missing." << endl;
                    }
                } else {
                    cout << "Invalid car ID or car is not rented." << endl;
                }
            } else if (choice == 3) {
                break;
            } else {
                cout << "Invalid choice. Please enter a valid option." << endl;
            }
        }

        cout << "\nThank you for using the Car Rental System!" << endl;
    }
};

// Main function
int main() {
    CarRentalSystem rentalSystem;

    auto car1 = make_shared<EconomyCar>("C001", "Toyota", "Camry", 60.0);
    auto car2 = make_shared<EconomyCar>("C002", "Honda", "Accord", 70.0);
    auto car3 = make_shared<LuxuryCar>("C003", "BMW", "7 Series", 150.0);

    rentalSystem.addCar(car1);
    rentalSystem.addCar(car2);
    rentalSystem.addCar(car3);

    rentalSystem.menu();
    return 0;
}
