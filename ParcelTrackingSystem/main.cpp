#include <iostream>
#include <string>
#ifdef _WIN32
#include <direct.h>
#endif
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;

// ------------------------------------------------------------
// Utility functions
// ------------------------------------------------------------
namespace Util
{
    string trim(const string &s)
    {
        size_t first = s.find_first_not_of(" \t\r\n");
        if (first == string::npos)
            return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, last - first + 1);
    }

    string readLine(const string &prompt)
    {
        string value;
        cout << prompt;
        getline(cin, value);
        return trim(value);
    }

    bool containsDelimiter(const string &value)
    {
        return value.find('|') != string::npos;
    }

    string readRequired(const string &prompt)
    {
        while (true)
        {
            string value = readLine(prompt);
            if (value.empty())
            {
                cout << "This field cannot be empty. Please try again.\n";
            }
            else if (containsDelimiter(value))
            {
                cout << "The character '|' is not allowed. Please try again.\n";
            }
            else
            {
                return value;
            }
        }
    }

    int readInt(const string &prompt, int low, int high)
    {
        while (true)
        {
            string s = readLine(prompt);
            stringstream ss(s);
            int value;
            char extra;
            if ((ss >> value) && !(ss >> extra) && value >= low && value <= high)
            {
                return value;
            }
            cout << "Enter a valid number from " << low << " to " << high << ".\n";
        }
    }

    double readDouble(const string &prompt, double low, double high)
    {
        while (true)
        {
            string s = readLine(prompt);
            stringstream ss(s);
            double value;
            char extra;
            if ((ss >> value) && !(ss >> extra) && value >= low && value <= high)
            {
                return value;
            }
            cout << "Enter a valid value from " << low << " to " << high << ".\n";
        }
    }

    vector<string> split(const string &line, char delimiter)
    {
        vector<string> parts;
        string item;
        stringstream ss(line);
        while (getline(ss, item, delimiter))
            parts.push_back(item);
        // Preserve a final empty field.
        if (!line.empty() && line[line.size() - 1] == delimiter)
            parts.push_back("");
        return parts;
    }

    string currentTimestamp()
    {
        time_t now = time(NULL);
        tm *local = localtime(&now);
        if (!local)
            return "Unknown time";
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
        return buffer;
    }

    string money(double amount)
    {
        ostringstream out;
        out << fixed << setprecision(2) << amount;
        return out.str();
    }

    string upper(const string &s)
    {
        string result = s;
        transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c)
                  { return static_cast<char>(toupper(c)); });
        return result;
    }
}

// ------------------------------------------------------------
// Model classes
// ------------------------------------------------------------
class User
{
protected:
    string userId;
    string name;
    string phone;
    string address;

public:
    User() {}
    User(const string &id, const string &n, const string &p, const string &a)
        : userId(id), name(n), phone(p), address(a) {}

    virtual ~User() {}

    string getId() const { return userId; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }

    virtual void displayProfile() const
    {
        cout << "ID: " << userId << "\nName: " << name
             << "\nPhone: " << phone << "\nAddress: " << address << "\n";
    }
};

class Customer : public User
{
public:
    Customer() {}
    Customer(const string &id, const string &n, const string &p, const string &a)
        : User(id, n, p, a) {}
};

class DeliveryAgent : public User
{
private:
    string serviceArea;

public:
    DeliveryAgent() {}
    DeliveryAgent(const string &id, const string &n, const string &p,
                  const string &a, const string &area)
        : User(id, n, p, a), serviceArea(area) {}

    string getServiceArea() const { return serviceArea; }

    void displayProfile() const override
    {
        User::displayProfile();
        cout << "Service area: " << serviceArea << "\n";
    }
};

struct TrackingEvent
{
    string status;
    string location;
    string timestamp;
    string note;

    TrackingEvent() {}
    TrackingEvent(const string &s, const string &l, const string &t, const string &n)
        : status(s), location(l), timestamp(t), note(n) {}
};

class Parcel
{
protected:
    string trackingId;
    string customerId;
    string senderName;
    string receiverName;
    string senderPhone;
    string receiverPhone;
    string source;
    string destination;
    double weight;
    double distance;
    string status;
    string agentId;
    string bookingTime;
    vector<TrackingEvent> history;

public:
    Parcel() : weight(0.0), distance(0.0), status("Booked") {}

    Parcel(const string &id, const string &custId, const string &sName,
           const string &rName, const string &sPhone, const string &rPhone,
           const string &src, const string &dest, double w, double d,
           const string &typeStatus, const string &agent, const string &booked)
        : trackingId(id), customerId(custId), senderName(sName),
          receiverName(rName), senderPhone(sPhone), receiverPhone(rPhone),
          source(src), destination(dest), weight(w), distance(d),
          status(typeStatus), agentId(agent), bookingTime(booked) {}

    virtual ~Parcel() {}

    virtual string getType() const = 0;
    virtual double calculateCharge() const = 0;

    string getTrackingId() const { return trackingId; }
    string getCustomerId() const { return customerId; }
    string getStatus() const { return status; }
    string getAgentId() const { return agentId; }
    string getSource() const { return source; }
    string getDestination() const { return destination; }
    double getWeight() const { return weight; }
    double getDistance() const { return distance; }
    const vector<TrackingEvent> &getHistory() const { return history; }

    void setAgentId(const string &id) { agentId = id; }

    bool isTerminal() const
    {
        return status == "Delivered" || status == "Returned";
    }

    bool canMoveTo(const string &next) const
    {
        if (status == "Booked")
            return next == "Picked Up";
        if (status == "Picked Up")
            return next == "In Transit";
        if (status == "In Transit")
            return next == "Out for Delivery";
        if (status == "Out for Delivery")
            return next == "Delivered" || next == "Delivery Failed";
        if (status == "Delivery Failed")
            return next == "Out for Delivery" || next == "Returned";
        return false;
    }

    bool updateStatus(const string &next, const string &location, const string &note)
    {
        if (!canMoveTo(next))
            return false;
        status = next;
        history.push_back(TrackingEvent(status, location, Util::currentTimestamp(), note));
        return true;
    }

    void addInitialEvent()
    {
        history.push_back(TrackingEvent("Booked", source, bookingTime, "Parcel booking created"));
    }

    void displaySummary() const
    {
        cout << left << setw(16) << trackingId
             << setw(16) << getType()
             << setw(20) << status
             << setw(16) << (agentId.empty() ? "Not assigned" : agentId)
             << "Rs. " << fixed << setprecision(2) << calculateCharge() << "\n";
    }

    void displayDetails() const
    {
        cout << "\n--------------- PARCEL DETAILS ---------------\n";
        cout << "Tracking ID       : " << trackingId << "\n";
        cout << "Parcel type       : " << getType() << "\n";
        cout << "Customer ID       : " << customerId << "\n";
        cout << "Sender            : " << senderName << " (" << senderPhone << ")\n";
        cout << "Receiver          : " << receiverName << " (" << receiverPhone << ")\n";
        cout << "Origin            : " << source << "\n";
        cout << "Destination       : " << destination << "\n";
        cout << "Weight            : " << weight << " kg\n";
        cout << "Distance          : " << distance << " km\n";
        cout << "Delivery type     : " << getType() << "\n";
        cout << "Current status    : " << status << "\n";
        cout << "Delivery agent    : " << (agentId.empty() ? "Not assigned" : agentId) << "\n";
        cout << "Estimated charge  : Rs. " << fixed << setprecision(2) << calculateCharge() << "\n";
        cout << "Booking time      : " << bookingTime << "\n";
        cout << "----------------------------------------------\n";
    }

    void displayHistory() const
    {
        cout << "\n--------------- TRACKING HISTORY ---------------\n";
        if (history.empty())
        {
            cout << "No tracking events available.\n";
            return;
        }
        for (size_t i = 0; i < history.size(); ++i)
        {
            cout << i + 1 << ". " << history[i].status
                 << " | " << history[i].location
                 << " | " << history[i].timestamp;
            if (!history[i].note.empty())
                cout << " | " << history[i].note;
            cout << "\n";
        }
        cout << "-------------------------------------------------\n";
    }

    // Pipe-delimited persistence record. User input is checked to forbid '|'.
    string serialize() const
    {
        ostringstream out;
        out << getType() << "|" << trackingId << "|" << customerId << "|"
            << senderName << "|" << receiverName << "|" << senderPhone << "|"
            << receiverPhone << "|" << source << "|" << destination << "|"
            << fixed << setprecision(3) << weight << "|" << distance << "|"
            << status << "|" << agentId << "|" << bookingTime;
        return out.str();
    }

    void setLoadedHistory(const vector<TrackingEvent> &events) { history = events; }
};

class StandardParcel : public Parcel
{
public:
    StandardParcel() {}
    StandardParcel(const string &id, const string &custId, const string &sName,
                   const string &rName, const string &sPhone, const string &rPhone,
                   const string &src, const string &dest, double w, double d,
                   const string &typeStatus, const string &agent, const string &booked)
        : Parcel(id, custId, sName, rName, sPhone, rPhone, src, dest, w, d,
                 typeStatus, agent, booked) {}

    string getType() const override { return "Standard"; }
    double calculateCharge() const override
    {
        return 50.0 + weight * 20.0 + distance * 0.10;
    }
};

class ExpressParcel : public Parcel
{
public:
    ExpressParcel() {}
    ExpressParcel(const string &id, const string &custId, const string &sName,
                  const string &rName, const string &sPhone, const string &rPhone,
                  const string &src, const string &dest, double w, double d,
                  const string &typeStatus, const string &agent, const string &booked)
        : Parcel(id, custId, sName, rName, sPhone, rPhone, src, dest, w, d,
                 typeStatus, agent, booked) {}

    string getType() const override { return "Express"; }
    double calculateCharge() const override
    {
        return 50.0 + weight * 20.0 + distance * 0.10 + 50.0;
    }
};

// ------------------------------------------------------------
// File persistence
// ------------------------------------------------------------
class FileManager
{
private:
    const string dataDir;

    string path(const string &file) const { return dataDir + "/" + file; }

public:
    FileManager() : dataDir("Data")
    {
#ifdef _WIN32
        _mkdir("Data");
#else
        mkdir("Data", 0777);
#endif
    }

    void loadCustomers(vector<Customer> &customers) const
    {
        ifstream in(path("customers.txt").c_str());
        string line;
        while (getline(in, line))
        {
            vector<string> p = Util::split(line, '|');
            if (p.size() == 4)
                customers.push_back(Customer(p[0], p[1], p[2], p[3]));
        }
    }

    void loadAgents(vector<DeliveryAgent> &agents) const
    {
        ifstream in(path("agents.txt").c_str());
        string line;
        while (getline(in, line))
        {
            vector<string> p = Util::split(line, '|');
            if (p.size() == 5)
                agents.push_back(DeliveryAgent(p[0], p[1], p[2], p[3], p[4]));
        }
    }

    void loadParcels(vector<unique_ptr<Parcel>> &parcels) const
    {
        ifstream in(path("parcels.txt").c_str());
        string line;
        while (getline(in, line))
        {
            vector<string> p = Util::split(line, '|');
            if (p.size() != 14)
                continue;

            string type = p[0];
            double weight = atof(p[9].c_str());
            double distance = atof(p[10].c_str());

            unique_ptr<Parcel> parcel;
            if (type == "Standard")
            {
                parcel.reset(new StandardParcel(p[1], p[2], p[3], p[4], p[5], p[6],
                                                p[7], p[8], weight, distance, p[11], p[12], p[13]));
            }
            else if (type == "Express")
            {
                parcel.reset(new ExpressParcel(p[1], p[2], p[3], p[4], p[5], p[6],
                                               p[7], p[8], weight, distance, p[11], p[12], p[13]));
            }
            else
            {
                continue;
            }

            vector<TrackingEvent> events;
            ifstream trackIn(path("tracking.txt").c_str());
            string eventLine;
            while (getline(trackIn, eventLine))
            {
                vector<string> e = Util::split(eventLine, '|');
                if (e.size() == 5 && e[0] == p[1])
                    events.push_back(TrackingEvent(e[1], e[2], e[3], e[4]));
            }
            parcel->setLoadedHistory(events);
            parcels.push_back(std::move(parcel));
        }
    }

    void saveCustomers(const vector<Customer> &customers) const
    {
        ofstream out(path("customers.txt").c_str());
        for (size_t i = 0; i < customers.size(); ++i)
            out << customers[i].getId() << "|" << customers[i].getName() << "|"
                << customers[i].getPhone() << "|" << customers[i].getAddress() << "\n";
    }

    void saveAgents(const vector<DeliveryAgent> &agents) const
    {
        ofstream out(path("agents.txt").c_str());
        for (size_t i = 0; i < agents.size(); ++i)
            out << agents[i].getId() << "|" << agents[i].getName() << "|"
                << agents[i].getPhone() << "|" << agents[i].getAddress() << "|"
                << agents[i].getServiceArea() << "\n";
    }

    void saveParcels(const vector<unique_ptr<Parcel>> &parcels) const
    {
        ofstream out(path("parcels.txt").c_str());
        for (size_t i = 0; i < parcels.size(); ++i)
            out << parcels[i]->serialize() << "\n";

        ofstream trackOut(path("tracking.txt").c_str());
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            const vector<TrackingEvent> &events = parcels[i]->getHistory();
            for (size_t j = 0; j < events.size(); ++j)
            {
                trackOut << parcels[i]->getTrackingId() << "|"
                         << events[j].status << "|" << events[j].location << "|"
                         << events[j].timestamp << "|" << events[j].note << "\n";
            }
        }
    }
};

// ------------------------------------------------------------
// Main application / service layer
// ------------------------------------------------------------
class CourierManagement
{
private:
    vector<Customer> customers;
    vector<DeliveryAgent> agents;
    vector<unique_ptr<Parcel>> parcels;
    FileManager files;
    int customerSerial;
    int agentSerial;
    int parcelSerial;

    Customer *findCustomer(const string &id)
    {
        for (size_t i = 0; i < customers.size(); ++i)
            if (customers[i].getId() == id)
                return &customers[i];
        return NULL;
    }

    DeliveryAgent *findAgent(const string &id)
    {
        for (size_t i = 0; i < agents.size(); ++i)
            if (agents[i].getId() == id)
                return &agents[i];
        return NULL;
    }

    Parcel *findParcel(const string &id)
    {
        for (size_t i = 0; i < parcels.size(); ++i)
            if (parcels[i]->getTrackingId() == id)
                return parcels[i].get();
        return NULL;
    }

    string nextId(const string &prefix, int serial, int width) const
    {
        ostringstream out;
        out << prefix << setw(width) << setfill('0') << serial;
        return out.str();
    }

    bool agentHasActiveParcel(const string &agentId) const
    {
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            if (parcels[i]->getAgentId() == agentId && !parcels[i]->isTerminal())
                return true;
        }
        return false;
    }

    bool customerHasParcel(const string &customerId) const
    {
        for (size_t i = 0; i < parcels.size(); ++i)
            if (parcels[i]->getCustomerId() == customerId)
                return true;
        return false;
    }

    void saveAll() const
    {
        files.saveCustomers(customers);
        files.saveAgents(agents);
        files.saveParcels(parcels);
    }

    void loadAll()
    {
        files.loadCustomers(customers);
        files.loadAgents(agents);
        files.loadParcels(parcels);

        customerSerial = 1;
        agentSerial = 1;
        parcelSerial = 1;

        for (size_t i = 0; i < customers.size(); ++i)
        {
            string id = customers[i].getId();
            if (id.size() > 4)
                customerSerial = max(customerSerial, atoi(id.substr(4).c_str()) + 1);
        }
        for (size_t i = 0; i < agents.size(); ++i)
        {
            string id = agents[i].getId();
            if (id.size() > 3)
                agentSerial = max(agentSerial, atoi(id.substr(3).c_str()) + 1);
        }
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            string id = parcels[i]->getTrackingId();
            if (id.size() >= 13)
                parcelSerial = max(parcelSerial, atoi(id.substr(7).c_str()) + 1);
        }
    }

    void printParcelHeader() const
    {
        cout << left << setw(16) << "Tracking ID" << setw(16) << "Type"
             << setw(20) << "Status" << setw(16) << "Agent" << "Charge\n";
        cout << string(82, '-') << "\n";
    }

    void registerCustomer()
    {
        cout << "\n========== CUSTOMER REGISTRATION ==========\n";
        string name = Util::readRequired("Full name: ");
        string phone = Util::readRequired("Phone number: ");
        string address = Util::readRequired("Address: ");

        string id = nextId("CUST", customerSerial++, 4);
        customers.push_back(Customer(id, name, phone, address));
        saveAll();
        cout << "\nRegistration successful. Your Customer ID is: " << id << "\n";
    }

    void bookParcel()
    {
        cout << "\n=============== BOOK A PARCEL ===============\n";
        string customerId = Util::readRequired("Customer ID: ");
        Customer *customer = findCustomer(customerId);
        if (!customer)
        {
            cout << "Customer ID not found. Please register first.\n";
            return;
        }

        string sender = customer->getName();
        string senderPhone = customer->getPhone();
        cout << "Sender: " << sender << "\n";

        string receiver = Util::readRequired("Receiver name: ");
        string receiverPhone = Util::readRequired("Receiver phone: ");
        string source = Util::readRequired("Pickup city / address: ");
        string destination = Util::readRequired("Destination city / address: ");
        double weight = Util::readDouble("Parcel weight in kg (0.01 to 1000): ", 0.01, 1000.0);
        double distance = Util::readDouble("Estimated distance in km (0 to 50000): ", 0.0, 50000.0);

        cout << "Delivery type:\n1. Standard\n2. Express\n";
        int type = Util::readInt("Choose type: ", 1, 2);

        time_t now = time(NULL);
        tm *local = localtime(&now);
        int year = local ? local->tm_year + 1900 : 2026;
        ostringstream idStream;
        idStream << "PAR" << year << setw(6) << setfill('0') << parcelSerial++;
        string trackingId = idStream.str();
        string booked = Util::currentTimestamp();

        unique_ptr<Parcel> parcel;
        if (type == 1)
        {
            parcel.reset(new StandardParcel(trackingId, customerId, sender, receiver,
                                            senderPhone, receiverPhone, source, destination, weight, distance,
                                            "Booked", "", booked));
        }
        else
        {
            parcel.reset(new ExpressParcel(trackingId, customerId, sender, receiver,
                                           senderPhone, receiverPhone, source, destination, weight, distance,
                                           "Booked", "", booked));
        }
        parcel->addInitialEvent();
        double charge = parcel->calculateCharge();
        parcels.push_back(std::move(parcel));
        saveAll();

        cout << "\n========== BOOKING CONFIRMATION ==========\n";
        cout << "Tracking ID      : " << trackingId << "\n";
        cout << "Customer ID      : " << customerId << "\n";
        cout << "Delivery type    : " << (type == 1 ? "Standard" : "Express") << "\n";
        cout << "Estimated charge : Rs. " << fixed << setprecision(2) << charge << "\n";
        cout << "Current status   : Booked\n";
        cout << "==========================================\n";
        cout << "Please save your tracking ID for future reference.\n";
    }

    void trackParcel() const
    {
        cout << "\n=============== TRACK A PARCEL ===============\n";
        string id = Util::readRequired("Enter tracking ID: ");
        const Parcel *parcel = NULL;
        for (size_t i = 0; i < parcels.size(); ++i)
            if (parcels[i]->getTrackingId() == id)
                parcel = parcels[i].get();

        if (!parcel)
        {
            cout << "No parcel found with tracking ID " << id << ".\n";
            return;
        }
        parcel->displayDetails();
        parcel->displayHistory();
    }

    void adminLogin()
    {
        cout << "\n================ ADMIN LOGIN ================\n";
        string username = Util::readRequired("Username: ");
        string password = Util::readRequired("Password: ");
        if (username != "admin" || password != "admin123")
        {
            cout << "Invalid admin credentials.\n";
            return;
        }
        adminMenu();
    }

    void addAgent()
    {
        cout << "\n========== ADD DELIVERY AGENT ==========\n";
        string name = Util::readRequired("Agent name: ");
        string phone = Util::readRequired("Phone number: ");
        string address = Util::readRequired("Address: ");
        string area = Util::readRequired("Service area: ");
        string id = nextId("AGT", agentSerial++, 4);
        agents.push_back(DeliveryAgent(id, name, phone, address, area));
        saveAll();
        cout << "Agent added successfully. Agent ID: " << id << "\n";
    }

    void viewAgents() const
    {
        cout << "\n================ DELIVERY AGENTS ================\n";
        if (agents.empty())
        {
            cout << "No agents registered.\n";
            return;
        }
        for (size_t i = 0; i < agents.size(); ++i)
        {
            cout << agents[i].getId() << " | " << agents[i].getName()
                 << " | " << agents[i].getPhone()
                 << " | Area: " << agents[i].getServiceArea() << "\n";
        }
    }

    void viewAllParcels() const
    {
        cout << "\n================ ALL PARCELS ================\n";
        if (parcels.empty())
        {
            cout << "No parcel bookings found.\n";
            return;
        }
        printParcelHeader();
        for (size_t i = 0; i < parcels.size(); ++i)
            parcels[i]->displaySummary();
    }

    void assignAgent()
    {
        cout << "\n========== ASSIGN DELIVERY AGENT ==========\n";
        string parcelId = Util::readRequired("Tracking ID: ");
        Parcel *parcel = findParcel(parcelId);
        if (!parcel)
        {
            cout << "Parcel not found.\n";
            return;
        }
        if (parcel->isTerminal())
        {
            cout << "Cannot assign an agent to a completed/returned parcel.\n";
            return;
        }
        if (!parcel->getAgentId().empty())
        {
            cout << "This parcel is already assigned to " << parcel->getAgentId() << ".\n";
            return;
        }
        if (agents.empty())
        {
            cout << "No delivery agents registered. Add an agent first.\n";
            return;
        }

        viewAgents();
        string agentId = Util::readRequired("Enter agent ID to assign: ");
        DeliveryAgent *agent = findAgent(agentId);
        if (!agent)
        {
            cout << "Agent ID not found.\n";
            return;
        }
        if (agentHasActiveParcel(agentId))
        {
            cout << "This agent already has an active parcel. Choose another agent.\n";
            return;
        }
        parcel->setAgentId(agentId);
        saveAll();
        cout << "Parcel " << parcelId << " assigned to " << agent->getName()
             << " (" << agentId << ").\n";
    }

    void viewUnassigned() const
    {
        cout << "\n============= UNASSIGNED PARCELS =============\n";
        bool found = false;
        printParcelHeader();
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            if (parcels[i]->getAgentId().empty() && !parcels[i]->isTerminal())
            {
                parcels[i]->displaySummary();
                found = true;
            }
        }
        if (!found)
            cout << "There are no unassigned active parcels.\n";
    }

    void revenueSummary() const
    {
        double total = 0.0;
        int delivered = 0;
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            if (parcels[i]->getStatus() == "Delivered")
            {
                total += parcels[i]->calculateCharge();
                ++delivered;
            }
        }
        cout << "\n=============== REVENUE SUMMARY ===============\n";
        cout << "Total bookings       : " << parcels.size() << "\n";
        cout << "Delivered parcels    : " << delivered << "\n";
        cout << "Revenue from delivered parcels (estimated): Rs. "
             << fixed << setprecision(2) << total << "\n";
        cout << "Note: This is a project estimate, not an accounting statement.\n";
    }

    void viewCustomers() const
    {
        cout << "\n================ CUSTOMERS ================\n";
        if (customers.empty())
        {
            cout << "No customers registered.\n";
            return;
        }
        for (size_t i = 0; i < customers.size(); ++i)
        {
            cout << customers[i].getId() << " | " << customers[i].getName()
                 << " | " << customers[i].getPhone()
                 << " | " << customers[i].getAddress() << "\n";
        }
    }

    void adminMenu()
    {
        while (true)
        {
            cout << "\n=============== ADMIN DASHBOARD ===============\n"
                 << "1. Add Delivery Agent\n"
                 << "2. View All Parcels\n"
                 << "3. Assign Delivery Agent\n"
                 << "4. View Unassigned Parcels\n"
                 << "5. View Revenue Summary\n"
                 << "6. View All Customers\n"
                 << "7. View Delivery Agents\n"
                 << "8. Logout\n";
            int choice = Util::readInt("Enter choice: ", 1, 8);
            switch (choice)
            {
            case 1:
                addAgent();
                break;
            case 2:
                viewAllParcels();
                break;
            case 3:
                assignAgent();
                break;
            case 4:
                viewUnassigned();
                break;
            case 5:
                revenueSummary();
                break;
            case 6:
                viewCustomers();
                break;
            case 7:
                viewAgents();
                break;
            case 8:
                cout << "Admin logged out.\n";
                return;
            }
        }
    }

    void agentLogin()
    {
        cout << "\n============= DELIVERY AGENT LOGIN =============\n";
        string id = Util::readRequired("Agent ID: ");
        DeliveryAgent *agent = findAgent(id);
        if (!agent)
        {
            cout << "Agent ID not found.\n";
            return;
        }
        agentMenu(*agent);
    }

    void viewAgentParcels(const string &agentId) const
    {
        cout << "\n============= ASSIGNED PARCELS =============\n";
        bool found = false;
        printParcelHeader();
        for (size_t i = 0; i < parcels.size(); ++i)
        {
            if (parcels[i]->getAgentId() == agentId && !parcels[i]->isTerminal())
            {
                parcels[i]->displaySummary();
                found = true;
            }
        }
        if (!found)
            cout << "No active parcels assigned to you.\n";
    }

    void updateAgentParcel(const string &agentId)
    {
        cout << "\n============= UPDATE PARCEL STATUS =============\n";
        string id = Util::readRequired("Tracking ID: ");
        Parcel *parcel = findParcel(id);
        if (!parcel)
        {
            cout << "Parcel not found.\n";
            return;
        }
        if (parcel->getAgentId() != agentId)
        {
            cout << "This parcel is not assigned to your agent ID.\n";
            return;
        }

        cout << "Current status: " << parcel->getStatus() << "\n";
        vector<string> options;
        if (parcel->getStatus() == "Booked")
            options.push_back("Picked Up");
        else if (parcel->getStatus() == "Picked Up")
            options.push_back("In Transit");
        else if (parcel->getStatus() == "In Transit")
            options.push_back("Out for Delivery");
        else if (parcel->getStatus() == "Out for Delivery")
        {
            options.push_back("Delivered");
            options.push_back("Delivery Failed");
        }
        else if (parcel->getStatus() == "Delivery Failed")
        {
            options.push_back("Out for Delivery");
            options.push_back("Returned");
        }

        if (options.empty())
        {
            cout << "No further status update is allowed.\n";
            return;
        }

        cout << "Allowed next status:\n";
        for (size_t i = 0; i < options.size(); ++i)
            cout << i + 1 << ". " << options[i] << "\n";
        int selected = Util::readInt("Select status: ", 1, static_cast<int>(options.size()));
        string location = Util::readRequired("Current location: ");
        string note = Util::readLine("Note (optional): ");
        if (Util::containsDelimiter(note))
        {
            cout << "The character '|' is not allowed in the note.\n";
            return;
        }

        if (parcel->updateStatus(options[selected - 1], location, note))
        {
            saveAll();
            cout << "Parcel status updated successfully to "
                 << parcel->getStatus() << ".\n";
        }
        else
        {
            cout << "Status update rejected.\n";
        }
    }

    void agentMenu(const DeliveryAgent &agent)
    {
        while (true)
        {
            cout << "\n=============== AGENT DASHBOARD ===============\n"
                 << "Logged in as: " << agent.getName() << " (" << agent.getId() << ")\n"
                 << "1. View Assigned Parcels\n"
                 << "2. Update Parcel Status\n"
                 << "3. Logout\n";
            int choice = Util::readInt("Enter choice: ", 1, 3);
            if (choice == 1)
                viewAgentParcels(agent.getId());
            else if (choice == 2)
                updateAgentParcel(agent.getId());
            else
            {
                cout << "Agent logged out.\n";
                return;
            }
        }
    }

    void customerMenu()
    {
        while (true)
        {
            cout << "\n=============== CUSTOMER MENU ===============\n"
                 << "1. Register Customer\n"
                 << "2. Book a New Parcel\n"
                 << "3. Track a Parcel\n"
                 << "4. Back to Main Menu\n";
            int choice = Util::readInt("Enter choice: ", 1, 4);
            if (choice == 1)
                registerCustomer();
            else if (choice == 2)
                bookParcel();
            else if (choice == 3)
                trackParcel();
            else
                return;
        }
    }

public:
    CourierManagement() : customerSerial(1), agentSerial(1), parcelSerial(1)
    {
        loadAll();
    }

    void run()
    {
        cout << "====================================================\n"
             << "       PARCEL TRACKING & DELIVERY MANAGEMENT\n"
             << "====================================================\n";
        cout << "Existing records loaded: " << customers.size() << " customers, "
             << agents.size() << " agents, " << parcels.size() << " parcels.\n";

        while (true)
        {
            cout << "\n================ MAIN MENU ================\n"
                 << "1. Customer Section\n"
                 << "2. Admin Login\n"
                 << "3. Delivery Agent Login\n"
                 << "4. Exit\n";
            int choice = Util::readInt("Enter choice: ", 1, 4);
            if (choice == 1)
                customerMenu();
            else if (choice == 2)
                adminLogin();
            else if (choice == 3)
                agentLogin();
            else
            {
                saveAll();
                cout << "Data saved. Thank you for using the system!\n";
                break;
            }
        }
    }
};

int main()
{
    CourierManagement system;
    system.run();
    return 0;
}
