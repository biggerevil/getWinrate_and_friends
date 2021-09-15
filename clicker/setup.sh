#!/bin/bash

# Firsts, lets deal with rvm (https://rvm.io/rvm/install)
# Import gpg keys for installing rvm
command curl -sSL https://rvm.io/pkuczynski.asc | gpg --import -
# Install rvm
\curl -sSL https://get.rvm.io | bash -s stable --ruby
# Run source. It's said to do in the end of installing rvm. Didn't bother to read what is does though I know what source does
source /usr/local/rvm/scripts/rvm

# Then let's clone project
# Return home
cd
# Clone project
git clone https://github.com/biggerevil/solo322.git

# Then let's run bundle
# Go to project
cd solo322
# Run bundle install
bundle install

# Install firefox and xvfb (for launching GUI-apps in background, I don't quite understand what it does, didn't bother with it either, but I know it's required) for making my ruby-progam to work
sudo apt install firefox xvfb

# Copy file example_credentials.rb, now with name credentials.rb
cp example_credentials.rb credentials.rb

# Outputting so user may know it's ready to work. I believe it says everything right
echo -e "\n\n\n Basic setup done! \n Write your info in credentials.rb and you can run \`ruby main.rb\`! \n\n\n"
