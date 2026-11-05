# Parcel Tracking & Delivery Management System

**Student:** Sonu Singh  
**Student ID:** B125122  
**Branch:** CSE  
**Course:** BTech  
**Academic Year:** 2026

A terminal-based C++11 mini project implementing parcel booking, tracking ID generation,
delivery-charge calculation, agent assignment, delivery status updates and file persistence.

## Requirements
- g++ (GCC) or another C++11-compatible compiler
- Windows, Linux or macOS terminal

## Compile
From this project folder:

```bash
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o parcel_system
```

On Windows MinGW:

```bash
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o parcel_system.exe
```

## Run
Linux/macOS:
```bash
./parcel_system
```

Windows:
```bash
parcel_system.exe
```

The program creates a `Data` folder automatically (if it does not exist) and stores:
- `Data/customers.txt`
- `Data/agents.txt`
- `Data/parcels.txt`
- `Data/tracking.txt`

Keep the executable in this project folder so that it uses the same `Data` directory.

## Demo admin login
- Username: `admin`
- Password: `admin123`

Delivery agents log in using their Agent ID (for example `AGT0001`). This is a classroom
simulation, not production-grade authentication.

## Main workflow
1. Customer registers and receives a Customer ID (`CUST0001`).
2. Customer books a parcel using that ID.
3. System generates a tracking ID such as `PAR2026000001`.
4. The system calculates the charge:
   - Base fee: Rs. 50
   - Weight rate: Rs. 20 per kg
   - Distance rate: Rs. 0.10 per km
   - Express priority fee: Rs. 50
5. Admin adds delivery agents and assigns an available agent to a parcel.
6. Agent updates the parcel through valid delivery statuses.
7. Customer tracks the parcel using its tracking ID.

## Status transition rules
- Booked -> Picked Up
- Picked Up -> In Transit
- In Transit -> Out for Delivery
- Out for Delivery -> Delivered OR Delivery Failed
- Delivery Failed -> Out for Delivery OR Returned
- Delivered and Returned are terminal statuses.

Every valid status update adds a timestamped tracking event.

## OOP concepts demonstrated
- Encapsulation: private/protected data and public methods
- Inheritance: Customer and DeliveryAgent derive from User
- Abstraction: Parcel is an abstract base class
- Runtime polymorphism: StandardParcel and ExpressParcel override calculateCharge()
- Composition: Parcel contains a vector of TrackingEvent objects
- Constructors, virtual destructors, STL vectors and file handling

## Important notes
- Enter ordinary text without the pipe character `|`; it is used as the file delimiter.
- The project uses manually entered estimated distance; it does not use GPS or live maps.
- Prices are sample academic rates, not actual courier tariffs.
- Data files are plain text and intended for a single-user classroom demonstration.
- For a clean demo, back up or remove the files inside `Data` before starting fresh.
