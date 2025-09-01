#!/bin/bash

# Run the encryption client four times using port 9091
# do not add a background character
for i in {1..5};
do 
	/home/ango/Code/OTP-Redemption/OTP-Redemption/enc_client msg$i key 9091
done
