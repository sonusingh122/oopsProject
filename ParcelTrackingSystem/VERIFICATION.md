# Verification performed

The project was compiled with:

```bash
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o parcel_system
```

Compilation completed successfully with no compiler warnings or errors in the test environment.

A scripted end-to-end smoke test exercised:
- Customer registration
- Express parcel booking and charge calculation
- Admin login
- Delivery-agent creation
- Agent assignment
- Status updates: Booked -> Picked Up -> In Transit -> Out for Delivery -> Delivered
- Tracking lookup and tracking-history display
- Application restart and loading saved records
- Parcel tracking-ID serial increment after restart

The smoke test completed successfully. These checks are development checks in the provided environment; compile and run the project on the computer used for your demonstration as well.
