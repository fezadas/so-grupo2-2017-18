/*
 * Copyright (c) 2018 Miguel Gamboa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package test;

import org.junit.jupiter.api.Test;
import util.iterator.FlatMapIterator;
import util.iterator.TakeWhileIterator;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.function.Predicate;

import static java.util.Arrays.asList;
import static org.junit.jupiter.api.Assertions.assertIterableEquals;
import static util.Queries.filter;
import static util.Queries.flatMap;
import static util.Queries.forEach;
import static util.Queries.generate;
import static util.Queries.iterate;
import static util.Queries.limit;
import static util.Queries.takeWhile;

public class QueriesTest {

    @Test
    public void testGenerator() {
        Iterable<Double> nrs = limit(generate(Math::random), 7);
        forEach(nrs, System.out::println);
    }

    @Test
    public void testIterate() {
        Iterable<Integer> expected = asList(1, 2, 3, 4, 5, 6, 7);
        Iterable<Integer> nrs = takeWhile(n -> n < 8, iterate(0, n -> ++n));
        assertIterableEquals(expected, nrs);
        forEach(nrs, System.out::println);
    }

    @Test
    public void testFilterWithNullElements() {
        // Arrange
        Iterable<String> strs = asList("ola", "super", null, "abc", null, "1234");
        Predicate<String> p = s -> s == null || s.length() == 3;
        Iterable<String> expected = asList("ola", null, "abc", null);
        // Act
        Iterable<String> actual = filter(p, strs);
        // Assert
        assertIterableEquals(expected, actual);
    }

    @Test
    public void testTakWhile() {
        Iterable<Integer> nrs = asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 17, 31);
        Iterable<Integer> expected = asList(1, 2, 3, 4, 5, 6, 7);
        Iterable<Integer> actual = takeWhile(n -> n < 8, nrs);
        assertIterableEquals(expected, actual);
    }
    @Test
    public void testTakFlatMap() {
        Iterable<Integer> nrs = asList(2, 5, 8, 11);
        Iterable<Integer> expected = asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        Iterable<Integer> actual = flatMap(n -> asList(n - 1, n, n + 1), nrs);
        assertIterableEquals(expected, actual);
    }
}