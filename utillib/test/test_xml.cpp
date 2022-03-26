/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <gtest/gtest.h>
#include "util/ixmlfile.h"

using namespace util::xml;

namespace {

constexpr std::string_view kNormalXml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<Root>\n"
    "    <Tag1>Text</Tag1>\n"
    "    <Tag2>123</Tag2>\n"
    "    <Tag3>1.23</Tag3>\n"
    "    <Tag4>True</Tag4>\n"
    "    <Tag5>False</Tag5>\n"
    "    <ChildList>\n"
    "        <Child child='Olle' order='0' condition='True'/>\n"
    "        <Child child='Pelle' order='1' condition='True'/>\n"
    "    </ChildList>\n"
    "</Root>";

constexpr std::string_view kMdComment =
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<Root xmlns='http://www.asam.net/mdf/v4/'>\n"
    "    <Tag1 >Text</Tag1>\n"
    "    <Tag2>123</Tag2>\n"
    "    <Tag3>1.23</Tag3>\n"
    "    <Tag4>True</Tag4>\n"
    "    <Tag5>False</Tag5>\n"
    "    <ChildList>\n"
    "        <Child child='Olle' order='0' condition='True'/>\n"
    "        <Child child='Pelle' order='1' condition='True'/>\n"
    "    </ChildList>\n"
    "</Root>";

}

namespace util::test {

TEST(IXmlFile, ParseString) //NOLINT
{
  std::unique_ptr<IXmlFile> f = CreateXmlFile();

  EXPECT_TRUE(f->ParseString(kNormalXml.data()));
  EXPECT_EQ(f->Property<std::string>("Tag1"), std::string("Text"));
  EXPECT_EQ(f->Property<int>("Tag2"), 123);
  EXPECT_DOUBLE_EQ(f->Property<double>("Tag3"), 1.23);
  EXPECT_TRUE(f->Property<bool>("Tag4"));
  EXPECT_FALSE(f->Property<bool>("Tag5"));

  const auto *list = f->GetNode("ChildList");
  ASSERT_TRUE(list != nullptr);
  IXmlNode::ChildList node_list;
  list->GetChildList(node_list);
  EXPECT_EQ(node_list.size(), 2);

  auto child = node_list[1]->Attribute<std::string>("child");
  EXPECT_STREQ(child.c_str(), "Pelle");
  auto order = node_list[1]->Attribute<int>("order");
  EXPECT_EQ(order, 1);
  auto condition = node_list[1]->Attribute<bool>("condition");
  EXPECT_TRUE(condition);

  EXPECT_EQ(f->RootName(), std::string("Root"));

}

TEST(IXmlFile, ParseMdComment) //NOLINT
{
  std::unique_ptr<IXmlFile> f(std::move(CreateXmlFile()));
  EXPECT_TRUE(f->ParseString(kMdComment.data()));
  EXPECT_EQ(f->Property<std::string>("Tag1"), std::string("Text"));
  EXPECT_EQ(f->Property<int>("Tag2"), 123);
  EXPECT_DOUBLE_EQ(f->Property<double>("Tag3"), 1.23);
  EXPECT_TRUE(f->Property<bool>("Tag4"));
  EXPECT_FALSE(f->Property<bool>("Tag5"));

  const auto *list = f->GetNode("ChildList");
  ASSERT_TRUE(list != nullptr);
  IXmlNode::ChildList node_list;
  list->GetChildList(node_list);
  EXPECT_EQ(node_list.size(), 2);

  auto child = node_list[1]->Attribute<std::string>("child");
  EXPECT_STREQ(child.c_str(), "Pelle");
  auto order = node_list[1]->Attribute<int>("order");
  EXPECT_EQ(order, 1);
  auto condition = node_list[1]->Attribute<bool>("condition");
  EXPECT_TRUE(condition);

  EXPECT_EQ(f->RootName(), std::string("Root"));

}
} // namespace util::test