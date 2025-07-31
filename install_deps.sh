#!/bin/bash
set -e

echo "Updating package lists..."
sudo apt update

echo "Installing packages from requirements.apt..."
xargs -a requirements.apt sudo apt install -y

