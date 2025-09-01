#!/bin/bash

# Decrypt the encrypted file starting with encrypted_word_* using port 9092
for file in encrypted_word_*; do
	[[ -f "$file" ]] || continue
	/home/ango/Code/OTP-Redemption/OTP-Redemption/dec_client $file key 9092 &
done

