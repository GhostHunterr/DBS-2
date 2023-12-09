/*
* Group: 1 
* 
* Member-1
	Name: Harshil Prajapati
*	Id: 175255215
* 
* Member-2: 
	Name: Hetav Manojkumar Mamtora
	Id: 172913212

* Member-3: 
	Name: Farhan Hussain Sarang
	Id: 172963217
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>

using oracle::occi::Environment;
using oracle::occi::Connection;
using oracle::occi::Number;
using namespace oracle::occi;
using namespace std;

struct ShoppingCart
{
	int product_id{ 0 };
	double price{ 0 };
	int quantity{ 0 };
};

const int MAX_CART_SIZE = 5;	// the max number of items in one customer order


//Procedures
void find_customer(Connection* conn, int customer_id, int& found);
void find_product(Connection* conn, int product_id, string& product_name, double& price);
void add_order(Connection* conn, int customer_id, int& new_order_id);
int generate_order_id(Connection* conn);
void add_order_item(Connection* conn, int order_id, int item_id, int product_id, int quantity, double price);
void customer_order(Connection* conn, int customer_id, int& order_id);
void display_order_status(Connection* conn, int order_id, string& status);
void cancel_order(Connection* conn, int customer_id, int& order_id);

//Functions
void displayOrderStatus(Connection* conn, int orderId, int customerId);
void cancelOrder(Connection* conn, int orderId, int customerId);

int mainMenu();
int subMenu();
int customerLogin(Connection* conn, int customerId);
int addToCart(Connection* conn, struct ShoppingCart cart[]);
double findProduct(Connection* conn, int product_id);
void displayProducts(struct ShoppingCart cart[], int productCount);
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount);


int main()
{
	// OCCI Variables
	Environment* env = nullptr;
	Connection* conn = nullptr;

	// Used Variables
	string str;
	string user = "dbs311_233zbb33";
	string pass = "22675128";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

	try
	{
		//Connection
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);

		std::cout << "Connection is Successful!" << endl;

		int option{};
		int custId{};
		do
		{
			option = mainMenu();

			if (option)
			{
				cout << "Enter the customer ID: ";
				cin >> custId;

				if (customerLogin(conn, custId) == 0) {
					cout << "The customer does not exist." << endl;
					//option = 0;
				}
				else {
					ShoppingCart cart[5];
					int count = addToCart(conn, cart);
					displayProducts(cart, count);
					checkout(conn, cart, custId, count);
					//option = 0;
				}
			}

		} while (option);

		cout << "Good bye...!" << endl;
		//Termination
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& e)
	{
		std::cout << "Error: " << e.what();
	}
	return 0;
}


// PL/SQL Stored Procedures

void find_customer(Connection* conn, int customer_id, int& found) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN find_customer(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, customer_id);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(found));
		stmt->executeUpdate();
		found = stmt->getInt(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		std::cout << "Error: " << e.what();
	}
}

void find_product(Connection* conn, int product_id, string& product_name, double& price) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN find_product(:1, :2, :3); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, product_id);
		stmt->registerOutParam(2, Type::OCCISTRING, sizeof(product_name));
		stmt->registerOutParam(3, Type::OCCIDOUBLE, sizeof(price));
		stmt->executeUpdate();
		product_name = stmt->getString(2);
		price = stmt->getDouble(3);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}

void add_order(Connection* conn, int customer_id, int& new_order_id) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN add_order(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, customer_id);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(new_order_id));
		stmt->executeUpdate();
		new_order_id = stmt->getInt(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}

int generate_order_id(Connection* conn) {
	int new_order_id = 0;
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN :1 := generate_order_id(); END;";
		stmt->setSQL(sql);
		stmt->registerOutParam(1, Type::OCCIINT, sizeof(new_order_id));
		stmt->executeUpdate();
		new_order_id = stmt->getInt(1);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
	return new_order_id;
}

void add_order_item(Connection* conn, int order_id, int item_id, int product_id, int quantity, double price) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN add_order_item(:1, :2, :3, :4, :5); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, order_id);
		stmt->setInt(2, item_id);
		stmt->setInt(3, product_id);
		stmt->setInt(4, quantity);
		stmt->setDouble(5, price);
		stmt->executeUpdate();
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}

void customer_order(Connection* conn, int customer_id, int& order_id) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN customer_order(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, customer_id);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(order_id));
		stmt->executeUpdate();
		order_id = stmt->getInt(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}

void display_order_status(Connection* conn, int order_id, string& status) {

	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN display_order_status(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, order_id);
		stmt->registerOutParam(2, Type::OCCISTRING, sizeof(status));
		stmt->executeUpdate();
		status = stmt->getString(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}

void cancel_order(Connection* conn, int customer_id, int& order_id) {
	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN cancel_order(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, order_id);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(order_id));
		stmt->executeUpdate();
		order_id = stmt->getInt(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}
}


// C++ Functions
// returns the integer value which is the selected option by the user from the menu

void displayOrderStatus(Connection* conn, int orderId, int customerId) {
	try {
		Statement* cstmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
		cstmt->setInt(1, customerId);
		cstmt->registerOutParam(2, Type::OCCIINT);
		cstmt->executeUpdate();

		int orderIdCheck = cstmt->getInt(2);
		conn->terminateStatement(cstmt);

		if (orderIdCheck == 0) {
			std::cout << "Order ID is not valid." << endl;
			return;
		}

		Statement* cstmtStatus = conn->createStatement("BEGIN display_order_status(:1, :2); END;");
		cstmtStatus->setInt(1, orderId);
		cstmtStatus->registerOutParam(2, Type::OCCISTRING);
		cstmtStatus->executeUpdate();

		string status = cstmtStatus->getString(2);
		conn->terminateStatement(cstmtStatus);

		if (status.empty()) {
			std::cout << "Order does not exist." << endl;
		}
		else {
			std::cout << "Order is " << status << "." << endl;
		}
	}
	catch (SQLException& e) {
		std::cout << "Error: " << e.what() << endl;
	}
}

void cancelOrder(Connection* conn, int orderId, int customerId) {
	try {
		Statement* cstmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
		cstmt->setInt(1, orderId);
		cstmt->registerOutParam(2, Type::OCCIINT);
		cstmt->executeUpdate();

		int orderIdCheck = cstmt->getInt(2);
		conn->terminateStatement(cstmt);

		if (orderIdCheck == 0) {
			cout << "Order ID is not valid." << endl;
			return;
		}

		Statement* cstmtCancel = conn->createStatement("BEGIN cancel_order(:1, :2); END;");
		cstmtCancel->setInt(1, orderId);
		cstmtCancel->registerOutParam(2, Type::OCCIINT);
		cstmtCancel->executeUpdate();

		int cancelValue = cstmtCancel->getInt(2);
		conn->terminateStatement(cstmtCancel);

		if (cancelValue == 1) {
			cout << "Order is canceled." << endl;
		}
		else {
			cout << "Order does not exist." << endl;
		}
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what() << endl;
	}
}

int mainMenu() {

	int option = -1;

	while (option != 0 && option != 1) {
		cout << "******************** Main Menu ********************" << endl;
		cout << "1) Login" << endl;
		cout << "0) Exit" << endl;
		if (option == -1)
		{
			cout << "Enter an option (0-1): ";
		}
		else if (option != 0 && option != 1) {
			cout << "You entered a wrong value. Enter an option (0-1): ";
		}
		cin >> option;
	}
	return option;
}

int subMenu() {
	int option = -1;

	while (option < 0 || option > 3) {
		cout << "******************** Customer Service Menu ********************" << endl;
		cout << "1) Place an order" << endl;
		cout << "2) Check an order status" << endl;
		cout << "3) Cancel an order" << endl;
		cout << "0) Exit" << endl;
		cout << "Enter an option (0-3): ";
		cin >> option;

		if (option < 0 || option > 3) {
			cout << "Invalid option. ";
		}
	}

	return option;
}

int customerLogin(Connection* conn, int customerId) {
	int found = 0;

	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN find_customer(:id, :found); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, customerId);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(int));
		stmt->executeUpdate();
		found = stmt->getInt(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}

	return found;
}

int addToCart(Connection* conn, struct ShoppingCart cart[]) {
	int productCount = 0;
	bool addMoreProducts = true;

	while (addMoreProducts && productCount < 5) {
		int productId, quantity;
		double price = 0.0;

		cout << "Enter the product ID: ";
		cin >> productId;

		price = findProduct(conn, productId);

		if (price == 0.0) {
			cout << "The product does not exist. Try again..." << endl;
			continue;
		}

		cout << "Product Price: " << price << endl;
		cout << "Enter the product Quantity: ";
		cin >> quantity;

		cart[productCount].product_id = productId;
		cart[productCount].price = price;
		cart[productCount].quantity = quantity;

		productCount++;

		cout << "Enter 1 to add more products or 0 to checkout: ";
		int option;
		cin >> option;

		if (option != 1) {
			addMoreProducts = false;
		}
	}

	return productCount;
}

double findProduct(Connection* conn, int product_id) {
	double price = 0.0;

	try {
		Statement* stmt = conn->createStatement();
		string sql = "BEGIN find_product(:1, :2); END;";
		stmt->setSQL(sql);
		stmt->setInt(1, product_id);
		stmt->registerOutParam(2, Type::OCCIDOUBLE);
		stmt->executeUpdate();

		price = stmt->getDouble(2);
		conn->terminateStatement(stmt);
	}
	catch (SQLException& e) {
		cout << "Error: " << e.what();
	}

	return price;
}

void displayProducts(struct ShoppingCart cart[], int productCount) {
	double totalAmount = 0.0;

	cout << "------- Ordered Products ---------" << endl;
	for (int i = 0; i < productCount; ++i) {
		cout << "---Item " << i + 1 << endl;
		cout << "Product ID: " << cart[i].product_id << endl;
		cout << "Price: " << cart[i].price << endl;
		cout << "Quantity: " << cart[i].quantity << endl;

		double productTotal = cart[i].price * cart[i].quantity;
		totalAmount += productTotal;

		cout << "---Item " << i + 1 << endl;
	}

	cout << "----------------------------------" << endl;
	cout << "Total: " << totalAmount << endl;
}

int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount) {
	char choice;
	do {
		//if customer want to confirm order or not
		cout << "Would you like to checkout ? (Y/y or N/n) ";
		cin >> choice;

		if (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n')
			cout << "Wrong input. Try again..." << endl;
	} while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n');

	if (choice == 'N' || choice == 'n') {
		cout << "The order is cancelled." << endl;
		return 0;
	}
	else {
		// for execution of SQL scripts
		Statement* stmt = conn->createStatement();
		// input parameters
		stmt->setSQL("BEGIN add_order(:1, :2); END;");
		int next_order_id;
		//setting IN parameter of stored procedure
		stmt->setInt(1, customerId);
		// setting type and size of the OUT parameter
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(next_order_id));

		// Calling the procedure
		stmt->executeUpdate();

		next_order_id = stmt->getInt(2);

		for (int i = 0; i < productCount; ++i) {
			stmt->setSQL("BEGIN add_order_item(:1, :2, :3, :4, :5); END;");

			// Setting IN parameters
			stmt->setInt(1, next_order_id);
			stmt->setInt(2, i + 1);
			stmt->setInt(3, cart[i].product_id);
			stmt->setInt(4, cart[i].quantity);
			stmt->setDouble(5, cart[i].price);

			// Calling the  procedure
			stmt->executeUpdate();
		}

		cout << "The order is successfully completed." << endl;

		// Termination statement
		conn->terminateStatement(stmt);

		return 1;
	}
}