public class ProcessPayment {
    private PaymentStrategy strategy;
    
    public void setPaymentStrategy(PaymentStrategy strategy){
        this.strategy = strategy;
    }

    public void checkout(double amount){
        if(strategy == null){
            throw new IllegalStateException("Payment method not selected");
        }

        strategy.pay(amount);
    }
}