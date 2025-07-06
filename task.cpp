#include <iostream>
using namespace std;
#include <vector>
#include <iomanip>
#include <stdexcept>

string getCurrentDate(){
    return "2025-07-10";
}
class Shippable{
public:
    virtual double getWeight() const = 0;
    virtual ~Shippable() {}
};

class Expirable{
public:
    virtual bool isExpired() const = 0;
    virtual string getExpiryDate() const = 0;
    virtual ~Expirable() {}
};

class ShippingInfo: public Shippable{
    double weight;
public:
    ShippingInfo(double w): weight(w) {}
    double getWeight()  const override{ return weight; }
};

class ExpiryInfo: public Expirable{
    string expiryDate;
public:
    ExpiryInfo(string e): expiryDate(e) {}
    string getExpiryDate() const override {return expiryDate;}
    bool isExpired() const override{
        return getCurrentDate() > expiryDate;
    }
};

class Product{
    string name;
    float price;
    int quantity;
public:
    Shippable* shippingInfo = nullptr;
    Expirable* expiryInfo = nullptr;

    Product(string name, float price, int quantity){
        this->name = name;
        this->price = price;
        this->quantity = quantity;
    }

    string getName(){
        return name;
    }

    float getPrice(){
        return price;
    }

    int getQuantity(){
        return quantity;
    }

    void updateQuantity(int newQuantity){
        quantity += newQuantity;
    }

    bool isShippable(){
        return shippingInfo != NULL;
    }

    bool isExpirable(){
        return expiryInfo != NULL;
    }

    ~Product(){
        delete shippingInfo;
        delete expiryInfo;
    }

};


class Customer{
    float balance;
public:
    Customer(float balance): balance(balance){};

    float getBalance(){
        return balance;
    }

    void UpdateBalance(float newBalance){
        balance += newBalance;        
    }

    bool canBuy(float price){
        return balance >= price;
    }

};

class  CartItem{
public:
    Product* product;
    int quantity;
    CartItem(Product* p, int q) : product(p), quantity(q){};
};

class Cart{
    vector<CartItem> cartItems;
public:

    void add(Product &item, int quantity){
        if( item.getQuantity() < quantity ){
            throw runtime_error("Not Sufficent Stocks");
        }

        if(item.isExpirable() && item.expiryInfo->isExpired()){
            throw runtime_error("Expired item, can\'t be added");
        }

        CartItem temp(&item, quantity);
        cartItems.push_back(temp);
    }

    int size(){
        return cartItems.size();
    }
    bool empty(){
        return cartItems.empty();
    }

    vector<CartItem>& getItems(){
        return this->cartItems;
    }


    float calSubtotal(){
        float subTotal = 0;
        for(int i=0; i<cartItems.size(); i++){
            float price =  cartItems[i].product->getPrice();
            int quantity = cartItems[i].quantity;
            subTotal += price * quantity;
        }
        return subTotal;
    }
};

class ShippingService{
    vector<CartItem> items;
public:
    ShippingService() = default;
    ShippingService(vector<CartItem> ShippedItems): items(ShippedItems){}

    double getShippingFees(){
        /*Not sure if there's a logic behind shippign fees that i'm missing from the question*/
        return 30;
    }

    void shipItmes(){
        double totalWeight = 0;
        string unit;
        cout<< "**Shipment Notice **\n";
        for(int i=0; i<items.size(); i++){
            cout<< items[i].quantity << left << setw(4) << "x"
                << left<< setw(20)<< items[i].product->getName();
            
            double weight = items[i].product->shippingInfo->getWeight();
            totalWeight += weight;
            unit = "g";
            if(weight >= 1000){ 
                weight /= 1000;
                unit = "kg";
            }
            cout<< weight<<left<<setw(20)<<unit<<endl;
        }

        if(totalWeight>1000){
            totalWeight /= 1000;
            unit = "kg";
        }

        cout<<left<<setw(24)<<"Total package weight"<< totalWeight<<unit<<endl;
    }

};


class CheckoutService{
public:
    void finalizeCheckout(vector<CartItem> &items ){
        for(int i=0; i < items.size(); i++){
            items[i].product->updateQuantity(-items[i].quantity) ;
        }
    }

    void checkout(Customer &customer, Cart &cart){
        if(cart.empty()){
            throw runtime_error("Empty Cart");
        }

        vector<CartItem> &items = cart.getItems();
        vector<CartItem> shippedItems;
       
        for(int i=0; i < items.size(); i++){
            if(items[i].product->isShippable()){
                shippedItems.push_back(items[i]);
            }
        }

        float subTotal = cart.calSubtotal();
        double shippingFees = 0;
        ShippingService ship;
        if(!shippedItems.empty()){
            ship= ShippingService(shippedItems);
            shippingFees = ship.getShippingFees();
        }

        double total = subTotal + shippingFees;

        if (customer.canBuy(total)){
            if(!shippedItems.empty()){
                ship.shipItmes();
            }
            finalizeCheckout(items);
            printReceipt(items, subTotal, shippingFees, total);
            customer.UpdateBalance(-total);
        }else{
            throw runtime_error("Insufficent Funds");
        }
    }

    void printReceipt(vector<CartItem> items, double subtotal, double fees, double amount){
        cout<<"\n** Checkout Recipt ** \n";
        for(int i=0; i<items.size(); i++){
            cout<<items[i].quantity<<left<<setw(4)<<"x"
                <<left<<setw(20)<< items[i].product->getName()
                << items[i].product->getPrice()<<endl;
        }
        cout<<"-----------------------\n"
        << left<<setw(24)<<"Subtotal"<<subtotal<<endl;
        if(fees){
            cout<< left<<setw(24)<<"Shipping fees"<<fees<<endl;
        }

        cout<< left<<setw(24)<<"Amount"<<amount<<endl;
    }
};

int main(){
    
    Customer seif(1500);
    cout<<"Current balance: \t"<<seif.getBalance()<<endl;

    // Stocks
    Product* scratchCard= new Product("scratchCard", 10, 50);
    Product* cheese = new Product("cheese",5,10);
        cheese->shippingInfo = new ShippingInfo(400);
        cheese->expiryInfo = new ExpiryInfo("2025-08-18");
    Product* TV = new Product("tv", 500, 3);
        TV -> shippingInfo = new ShippingInfo(1100);
    Product* biscuits = new Product("biscuits",3, 30 );
        biscuits -> expiryInfo = new ExpiryInfo("2025-08-13");
    Product *book = new Product("book", 100,50);
        book->shippingInfo = new ShippingInfo(50);
        
    // Best Case
    Cart cart;
    cart.add(*scratchCard, 5);
    cart.add(*cheese, 5);
    cart.add(*TV, 2);
    cart.add(*biscuits, 5);
    cart.add(*book, 2);
    CheckoutService c;
    c.checkout(seif ,cart);
    cout<< "\nYour balance is now "<<seif.getBalance();

    cout<<"\n------------------------------------------------------\n";
    
    // insufficent funds 
    Cart cart2;
    try{
        cout<<"\n\tBefore Trying to add stocks > customer's balance\n";
        cout<< "balance: "<<seif.getBalance()<<"\nTVs in stock: "<< TV->getQuantity()<<"\nBooks in stock: "<<book->getQuantity()<<"\n";
        cart2.add(*TV, 1);
        cart2.add(*book, 5);
        c.checkout(seif, cart2);
    }catch(exception &e){
        cout<<"\n\tAfter Trying to add stocks > customer's balance\n";
        cout<<"Error Message:"<< e.what(); 
        cout<< "\nbalance: "<<seif.getBalance()<<"\nTVs in stock: "<< TV->getQuantity()<<"\nBooks in stock: "<<book->getQuantity()<<"\n";
    }

    cout<<"\n------------------------------------------------------\n";
    //Empty Cart
    Cart emptyCart;
    try{
        cout<<"\n\tBefore Trying to checkout empty cart\n";
        c.checkout(seif,emptyCart);
    }catch(exception &e){
        cout<<"\n\tAfter Trying to checkout empty cart\n";
        cout<<"Error Message:"<< e.what(); 
        cout<< "\nbalance: "<<seif.getBalance()<<endl;
    }
    
    cout<<"\n------------------------------------------------------\n";
    // Adding Expired product to a cart
    Product* expiredBiscuits = new Product("Expired Biscuits",3, 30 );
        expiredBiscuits -> expiryInfo = new ExpiryInfo("2024-08-13");
    try{
        cout<<"\n\tAdding Expired Product\n";
        cout<< "\nbalance: "<<seif.getBalance()<<endl;
        emptyCart.add(*expiredBiscuits, 5);
        c.checkout(seif, emptyCart);
    }catch(exception &e){
        cout<<"\n\tFailed Adding Expired Product\n";
        cout<<"Error Message:"<< e.what(); 
        cout<< "\nbalance: "<<seif.getBalance()<<endl;
    }

    cout<<"\n------------------------------------------------------\n";
    // Adding quantity to cart > stored
    try{
        cout<<"\n\tAdding quantity > stock level to the cart\n";
        cout<< "\nbalance: "<<seif.getBalance()<<endl
        << "TV Quantity: "<< TV->getQuantity()<<endl;
        emptyCart.add(*TV, 5);
    }catch(exception &e){
        cout<<"\n\tFailed Adding quantity > stock level to the cart\n";
        cout<<"Error Message:"<< e.what(); 
        cout<< "\nbalance: "<<seif.getBalance()<<endl
        << "TV Quantity: "<< TV->getQuantity()<<endl;
    }

    system("pause");

}   