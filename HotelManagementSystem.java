import java.util.*; 
 
class Customer { 
    int id; 
    String name; 
    String phone; 
    String email; 
 
    Customer(int id, String name, String phone, String email) { 
        this.id = id; 
        this.name = name; 
        this.phone = phone; 
        this.email = email; 
    } 
 
    public String toString() { 
        return "Customer ID: " + id + ", Name: " + name + ", Phone: " + phone + ", Email: " + 
email; 
    } 
} 
 
class Room { 
    int roomNumber; 
    String type; 
    boolean isBooked; 
    double pricePerDay; 
    String features; 
 
    Room(int roomNumber, String type, double pricePerDay, String features) { 
        this.roomNumber = roomNumber; 
        this.type = type; 
        this.pricePerDay = pricePerDay; 
        this.features = features; 
        this.isBooked = false; 
    } 
 
    public String toString() { 
        String status = isBooked ? "Booked" : "Available"; 
        return "Room No: " + roomNumber + ", Type: " + type + ", Price/Day: $" + pricePerDay +  
               ", Features: " + features + ", Status: " + status; 
    } 
} 
 
class Booking { 
    Customer customer; 
    Room room; 
    int days; 
    Date checkInDate; 
    Date checkOutDate; 
    int bookingId; 
    static int bookingCounter = 1000; 
 
    Booking(Customer customer, Room room, int days, Date checkInDate) { 
        this.customer = customer; 
        this.room = room; 
        this.days = days; 
        this.checkInDate = checkInDate; 
        this.checkOutDate = new Date(checkInDate.getTime() + (long) days * 24 * 60 * 60 * 
1000); 
        this.bookingId = ++bookingCounter; 
        room.isBooked = true; 
    } 
 
    public double calculateBill() { 
        return room.pricePerDay * days; 
    } 
 
    public double calculateBillWithTax() { 
        double bill = calculateBill(); 
        double tax = bill * 0.18; // 18% tax 
        return bill + tax; 
    } 
 
    public String toString() { 
        return "Booking ID: " + bookingId + "\n" + 
               customer.name + " booked Room " + room.roomNumber +  
               " for " + days + " days.\n" + 
               "Check-in: " + checkInDate + "\n" + 
               "Check-out: " + checkOutDate + "\n" + 
               "Total Bill (with 18% tax): $" + String.format("%.2f", calculateBillWithTax()); 
    } 
} 
 
public class HotelManagementSystem { 
    static ArrayList<Customer> customers = new ArrayList<>(); 
    static ArrayList<Room> rooms = new ArrayList<>(); 
    static ArrayList<Booking> bookings = new ArrayList<>(); 
    static Scanner sc = new Scanner(System.in); 
 
    public static void main(String[] args) { 
        // Initialize with sample data 
        initializeSampleData(); 
 
        while (true) { 
            System.out.println("\n========== HOTEL MANAGEMENT SYSTEM =========="); 
            System.out.println("1. Add Customer"); 
            System.out.println("2. View Customers"); 
            System.out.println("3. Add Room"); 
            System.out.println("4. View Available Rooms"); 
            System.out.println("5. View All Rooms"); 
            System.out.println("6. Book Room"); 
            System.out.println("7. View Bookings"); 
            System.out.println("8. Cancel Booking"); 
            System.out.println("9. Search Customer"); 
            System.out.println("10. Search Room"); 
            System.out.println("11. View Booking Details by ID"); 
            System.out.println("12. View Revenue Report"); 
            System.out.println("13. Exit"); 
            System.out.print("Enter your choice: "); 
             
            try { 
                int choice = sc.nextInt(); 
                sc.nextLine(); // Consume newline 
                 
                switch (choice) { 
                    case 1 -> addCustomer(); 
                    case 2 -> viewCustomers(); 
                    case 3 -> addRoom(); 
                    case 4 -> viewAvailableRooms(); 
                    case 5 -> viewAllRooms(); 
                    case 6 -> bookRoom(); 
                    case 7 -> viewBookings(); 
                    case 8 -> cancelBooking(); 
                    case 9 -> searchCustomer(); 
                    case 10 -> searchRoom(); 
                    case 11 -> viewBookingDetails(); 
                    case 12 -> viewRevenueReport(); 
                    case 13 -> { 
                        System.out.println("Thank you for using Hotel Management System!"); 
                        return; 
                    } 
                    default -> System.out.println("Invalid choice! Please try again."); 
                } 
            } catch (InputMismatchException e) { 
                System.out.println("Please enter a valid number!"); 
                sc.nextLine(); // Clear invalid input 
            } 
        } 
    } 
 
    static void initializeSampleData() { 
        // Add sample rooms 
        rooms.add(new Room(101, "Standard", 1000, "AC, TV, WiFi")); 
        rooms.add(new Room(102, "Deluxe", 2000, "AC, TV, WiFi, Mini-bar, Jacuzzi")); 
        rooms.add(new Room(103, "Standard", 1000, "AC, TV")); 
        rooms.add(new Room(201, "Suite", 3500, "AC, TV, WiFi, Kitchenette, Balcony")); 
        rooms.add(new Room(202, "Deluxe", 2000, "AC, TV, WiFi, Sea View")); 
         
        // Add sample customers 
        customers.add(new Customer(1, "John Doe", "1234567890", "john@email.com")); 
        customers.add(new Customer(2, "Jane Smith", "9876543210", "jane@email.com")); 
    } 
 
    static void addCustomer() { 
        System.out.println("\n--- Add New Customer ---"); 
        try { 
            System.out.print("Enter Customer ID: "); 
            int id = sc.nextInt(); 
            sc.nextLine(); 
             
            // Check if customer ID already exists 
            for (Customer c : customers) { 
                if (c.id == id) { 
                    System.out.println("Customer ID already exists!"); 
                    return; 
                } 
            } 
             
            System.out.print("Enter Name: "); 
            String name = sc.nextLine(); 
            System.out.print("Enter Phone: "); 
            String phone = sc.nextLine(); 
            System.out.print("Enter Email: "); 
            String email = sc.nextLine(); 
             
            customers.add(new Customer(id, name, phone, email)); 
            System.out.println("Customer added successfully!"); 
        } catch (InputMismatchException e) { 
            System.out.println("Invalid input! Please enter correct data."); 
            sc.nextLine(); 
        } 
    } 
 
    static void viewCustomers() { 
        System.out.println("\n--- Customer List ---"); 
        if (customers.isEmpty()) { 
            System.out.println("No customers found."); 
            return; 
        } 
        for (Customer c : customers) { 
            System.out.println(c); 
        } 
    } 
 
    static void addRoom() { 
        System.out.println("\n--- Add New Room ---"); 
        try { 
            System.out.print("Enter Room Number: "); 
            int roomNumber = sc.nextInt(); 
            sc.nextLine(); 
             
            // Check if room number already exists 
            for (Room r : rooms) { 
                if (r.roomNumber == roomNumber) { 
                    System.out.println("Room number already exists!"); 
                    return; 
                } 
            } 
             
            System.out.print("Enter Room Type (Standard/Deluxe/Suite): "); 
            String type = sc.nextLine(); 
            System.out.print("Enter Price per Day: "); 
            double price = sc.nextDouble(); 
            sc.nextLine(); 
            System.out.print("Enter Features: "); 
            String features = sc.nextLine(); 
             
            rooms.add(new Room(roomNumber, type, price, features)); 
            System.out.println("Room added successfully!"); 
        } catch (InputMismatchException e) { 
            System.out.println("Invalid input! Please enter correct data."); 
            sc.nextLine(); 
        } 
    } 
 
    static void viewAvailableRooms() { 
        System.out.println("\n--- Available Rooms ---"); 
        boolean found = false; 
        for (Room r : rooms) { 
            if (!r.isBooked) { 
                System.out.println(r); 
                found = true; 
            } 
        } 
        if (!found) { 
            System.out.println("No rooms available at the moment."); 
        } 
    } 
 
    static void viewAllRooms() { 
        System.out.println("\n--- All Rooms ---"); 
        if (rooms.isEmpty()) { 
            System.out.println("No rooms added yet."); 
            return; 
        } 
        for (Room r : rooms) { 
            System.out.println(r); 
        } 
    } 
 
    static void bookRoom() { 
        System.out.println("\n--- Book a Room ---"); 
         
        // Show available rooms first 
        viewAvailableRooms(); 
         
        try { 
            System.out.print("\nEnter Customer ID: "); 
            int customerId = sc.nextInt(); 
             
            Customer customer = null; 
            for (Customer c : customers) { 
                if (c.id == customerId) { 
                    customer = c; 
                    break; 
                } 
            } 
             
            if (customer == null) { 
                System.out.println("Customer not found! Please add customer first."); 
                return; 
            } 
             
            System.out.print("Enter Room Number: "); 
            int roomNumber = sc.nextInt(); 
             
            Room room = null; 
            for (Room r : rooms) { 
                if (r.roomNumber == roomNumber && !r.isBooked) { 
                    room = r; 
                    break; 
                } 
            } 
             
            if (room == null) { 
                System.out.println("Room not available or invalid room number!"); 
                return; 
            } 
             
            System.out.print("Enter Number of Days: "); 
            int days = sc.nextInt(); 
             
            if (days <= 0) { 
                System.out.println("Invalid number of days!"); 
                return; 
            } 
             
            Date checkInDate = new Date(); // Current date 
            Booking booking = new Booking(customer, room, days, checkInDate); 
            bookings.add(booking); 
             
            System.out.println("\nBooking Successful!"); 
            System.out.println(booking); 
             
        } catch (InputMismatchException e) { 
            System.out.println("Invalid input! Please enter correct data."); 
            sc.nextLine(); 
        } 
    } 
 
    static void viewBookings() { 
        System.out.println("\n--- All Bookings ---"); 
        if (bookings.isEmpty()) { 
            System.out.println("No bookings found."); 
            return; 
        } 
        for (Booking b : bookings) { 
            System.out.println(b); 
            System.out.println("-------------------"); 
        } 
    } 
 
    static void cancelBooking() { 
        System.out.println("\n--- Cancel Booking ---"); 
        System.out.print("Enter Booking ID: "); 
        try { 
            int bookingId = sc.nextInt(); 
             
            Booking bookingToCancel = null; 
            for (Booking b : bookings) { 
                if (b.bookingId == bookingId) { 
                    bookingToCancel = b; 
                    break; 
                } 
            } 
             
            if (bookingToCancel == null) { 
                System.out.println("Booking not found!"); 
                return; 
            } 
             
            bookingToCancel.room.isBooked = false; 
            bookings.remove(bookingToCancel); 
            System.out.println("Booking cancelled successfully!"); 
             
        } catch (InputMismatchException e) { 
            System.out.println("Invalid input!"); 
            sc.nextLine(); 
        } 
    } 
 
    static void searchCustomer() { 
        System.out.println("\n--- Search Customer ---"); 
        System.out.print("Enter Customer Name or ID: "); 
        String searchTerm = sc.nextLine(); 
         
        boolean found = false; 
        for (Customer c : customers) { 
            if (String.valueOf(c.id).equals(searchTerm) ||  
                c.name.toLowerCase().contains(searchTerm.toLowerCase())) { 
                System.out.println(c); 
                found = true; 
            } 
        } 
         
        if (!found) { 
            System.out.println("Customer not found."); 
        } 
    } 
 
    static void searchRoom() { 
        System.out.println("\n--- Search Room ---"); 
        System.out.print("Enter Room Number or Type: "); 
        String searchTerm = sc.nextLine(); 
         
        boolean found = false; 
        for (Room r : rooms) { 
            if (String.valueOf(r.roomNumber).equals(searchTerm) ||  
                r.type.toLowerCase().contains(searchTerm.toLowerCase())) { 
                System.out.println(r); 
                found = true; 
            } 
        } 
         
        if (!found) { 
            System.out.println("Room not found."); 
        } 
    } 
 
    static void viewBookingDetails() { 
        System.out.println("\n--- View Booking Details ---"); 
        System.out.print("Enter Booking ID: "); 
        try { 
            int bookingId = sc.nextInt(); 
             
            for (Booking b : bookings) { 
                if (b.bookingId == bookingId) { 
                    System.out.println(b); 
                    return; 
                } 
            } 
             
            System.out.println("Booking not found!"); 
             
        } catch (InputMismatchException e) { 
            System.out.println("Invalid input!"); 
            sc.nextLine(); 
        } 
    } 
 
    static void viewRevenueReport() { 
        System.out.println("\n--- Revenue Report ---"); 
        if (bookings.isEmpty()) { 
            System.out.println("No bookings yet. Revenue: $0.00"); 
            return; 
        } 
         
        double totalRevenue = 0; 
        int totalBookings = bookings.size(); 
        int roomsBooked = 0; 
         
        for (Booking b : bookings) { 
            totalRevenue += b.calculateBillWithTax(); 
            roomsBooked++; 
        } 
         
        System.out.println("Total Bookings: " + totalBookings); 
        System.out.println("Total Rooms Booked: " + roomsBooked); 
        System.out.println("Total Revenue (with tax): $" + String.format("%.2f", totalRevenue)); 
        System.out.println("Average Revenue per Booking: $" +  
                          String.format("%.2f", totalRevenue / totalBookings)); 
    } 
} 