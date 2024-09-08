import yfinance as yf
import numpy as np
import subprocess
import os

def fetch_data(tickers):
    data = {}
    for ticker in tickers:
        stock = yf.Ticker(ticker)
        hist = stock.history(period="1y")  # Fetching one year of historical data
        if hist.empty:
            print(f"No data found for ticker: {ticker}")
            data[ticker] = None
        else:
            try:
                data[ticker] = hist['Close'].iloc[-1]  # Get the last closing price
            except IndexError:
                print(f"Error accessing data for ticker: {ticker}")
                data[ticker] = None
    return data

def calculate_volatility(ticker):
    stock = yf.Ticker(ticker)
    hist = stock.history(period="1y")  # Fetching one year of historical data
    if hist.empty:
        print(f"No historical data found for ticker: {ticker}")
        return None

    hist['Return'] = hist['Close'].pct_change()  # Calculate daily returns
    hist = hist.dropna()  # Drop rows with NaN values

    daily_volatility = hist['Return'].std()  # Standard deviation of daily returns
    annual_volatility = daily_volatility * np.sqrt(252)  # Annualize volatility assuming 252 trading days in a year

    return annual_volatility

def fetch_us_yield():
    us_yield_ticker = "^TNX"  # 10-Year US Treasury Note Yield
    us_yield_data = fetch_data([us_yield_ticker])
    us_yield = us_yield_data.get(us_yield_ticker)
    return us_yield

def run_cpp_program(stock_price, strike_price, time_to_maturity, risk_free_rate, volatility):
    cpp_executable = os.path.join(os.getcwd(), 'black_scholes.exe')  # Path to the C++ executable
    command = [
        cpp_executable,
        str(stock_price),
        str(strike_price),
        str(time_to_maturity),
        str(risk_free_rate),
        str(volatility)
    ]
    try:
        result = subprocess.run(command, capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running C++ program: {e}")
        return None

def main():
    stock_tickers = input("Enter stock tickers separated by commas (e.g., AAPL,MSFT,GOOGL): ").split(',')
    stock_data = fetch_data(stock_tickers)
    us_yield = fetch_us_yield()

    print("Stock Data:", stock_data)
    print("US 10Y Yield:", us_yield)

    # Take dynamic inputs from the user
    stock_ticker = input("Enter the stock ticker for option pricing: ")
    stock_price = stock_data.get(stock_ticker.upper())
    
    if stock_price is None:
        print(f"Stock ticker {stock_ticker} not found or has no data. Exiting.")
        return

    strike_price = float(input("Enter the strike price: "))
    time_to_maturity = float(input("Enter the time to maturity (in years): "))
    risk_free_rate = float(input("Enter the risk-free rate (as a decimal): "))

    # Calculate volatility automatically
    volatility = calculate_volatility(stock_ticker)
    if volatility is None:
        print("Volatility could not be calculated. Exiting.")
        return

    print(f"Calculated Volatility: {volatility}")

    output = run_cpp_program(stock_price, strike_price, time_to_maturity, risk_free_rate, volatility)
    if output:
        print(output)

if __name__ == "__main__":
    main()
