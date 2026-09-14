from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_lilycove_tm_stock_is_resolved_before_building_the_shop_menu():
    shop = (ROOT / "src/shop_criteria.c").read_text()

    assert "ResolveForkLilycoveTmShopItem(item)" in shop


def test_lilycove_random_tm_sales_skip_the_quantity_selector():
    shop = (ROOT / "src/shop.c").read_text()
    single_copy_condition = (
        "GetItemImportance(itemId) || IsForkLilycoveTmShopItem(itemId)"
    )

    assert single_copy_condition in shop
