package main

import (
	"encoding/gob"
	"fmt"
	"io/ioutil"
	"os"

	"github.com/pelletier/go-toml"
	"golang.org/x/oauth2"
)

type Config struct {
	Connection struct {
		LocalAddress  string
		RemoteAddress string
		AllowedNames  []string
	}
}

func SaveToken(token *oauth2.Token) error {
	return SaveTokenVariant(token, "")
}

func SaveTokenVariant(token *oauth2.Token, variant string) error {
	f, err := os.Create(fmt.Sprintf("token%s.gob", variant))
	if err != nil {
		return err
	}
	defer f.Close()
	enc := gob.NewEncoder(f)
	return enc.Encode(token)
}

func LoadToken() (*oauth2.Token, error) {
	return LoadTokenVariant("")
}

func LoadTokenVariant(variant string) (*oauth2.Token, error) {
	f, err := os.Open(fmt.Sprintf("token%s.gob", variant))
	if err != nil {
		return nil, err
	}
	defer f.Close()
	dec := gob.NewDecoder(f)
	tkn := &oauth2.Token{}
	return tkn, dec.Decode(tkn)
}

func LoadConfig() (Config, error) {
	c := Config{}
	if _, err := os.Stat("config.toml"); err != nil {
		return c, err
	}
	data, err := ioutil.ReadFile("config.toml")
	if err != nil {
		return c, err
	}
	if err := toml.Unmarshal(data, &c); err != nil {
		return c, err
	}
	if c.Connection.LocalAddress == "" {
		c.Connection.LocalAddress = "0.0.0.0:19132"
	}
	data, _ = toml.Marshal(c)
	//if err := ioutil.WriteFile("config.toml", data, 0644); err != nil {
	//	return c, err
	//}
	return c, nil
}
