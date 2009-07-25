// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Ashish Myles (marcianx@gmail.com)
*/

#include "bezier_triangle_patch.h"
#include "metadata_keys.h"
#include "primitive_validation.h"
#include "selection.h"
#include "string_cast.h"

#include <numeric>

namespace k3d
{

namespace bezier_triangle_patch
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& PatchFirstPoints,
	const mesh::orders_t& PatchOrders,
	const mesh::selection_t& PatchSelections,
	const mesh::materials_t& PatchMaterials,
	const mesh::indices_t& PatchPoints,
	const mesh::weights_t& PatchPointWeights,
	const mesh::table_t& ConstantAttributes,
	const mesh::table_t& UniformAttributes,
	const mesh::table_t& VaryingAttributes
		) :
	patch_first_points(PatchFirstPoints),
	patch_orders(PatchOrders),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	constant_attributes(ConstantAttributes),
	uniform_attributes(UniformAttributes),
	varying_attributes(VaryingAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::indices_t& PatchFirstPoints,
	mesh::orders_t& PatchOrders,
	mesh::selection_t& PatchSelections,
	mesh::materials_t& PatchMaterials,
	mesh::indices_t& PatchPoints,
	mesh::weights_t& PatchPointWeights,
	mesh::table_t& ConstantAttributes,
	mesh::table_t& UniformAttributes,
	mesh::table_t& VaryingAttributes
		) :
	patch_first_points(PatchFirstPoints),
	patch_orders(PatchOrders),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	constant_attributes(ConstantAttributes),
	uniform_attributes(UniformAttributes),
	varying_attributes(VaryingAttributes)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("bezier_triangle_patch");

	primitive* const result = new primitive(
		generic_primitive.structure["uniform"].create<mesh::indices_t >("patch_first_points"),
		generic_primitive.structure["uniform"].create<mesh::orders_t >("patch_orders"),
		generic_primitive.structure["uniform"].create<mesh::selection_t >("patch_selections"),
		generic_primitive.structure["uniform"].create<mesh::materials_t >("patch_materials"),
		generic_primitive.structure["varying"].create<mesh::indices_t >("patch_points"),
		generic_primitive.structure["varying"].create<mesh::weights_t >("patch_point_weights"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->patch_selections.set_metadata_value(metadata::key::selection_component(), string_cast(selection::UNIFORM));
	result->patch_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "bezier_triangle_patch")
		return 0;

	try
	{
		require_valid_primitive(Primitive);

		const table& uniform_structure = require_structure(Primitive, "uniform");
		const table& varying_structure = require_structure(Primitive, "varying");

		const table& constant_attributes = require_attributes(Primitive, "constant");
		const table& uniform_attributes = require_attributes(Primitive, "uniform");
		const table& varying_attributes = require_attributes(Primitive, "varying");

		const mesh::indices_t& patch_first_points = require_array<mesh::indices_t >(Primitive, uniform_structure, "patch_first_points");
		const mesh::orders_t& patch_orders = require_array<mesh::orders_t >(Primitive, uniform_structure, "patch_orders");
		const mesh::selection_t& patch_selections = require_array<mesh::selection_t >(Primitive, uniform_structure, "patch_selections");
		const mesh::materials_t& patch_materials = require_array<mesh::materials_t >(Primitive, uniform_structure, "patch_materials");
		const mesh::indices_t& patch_points = require_array<mesh::indices_t >(Primitive, varying_structure, "patch_points");
		const mesh::weights_t& patch_point_weights = require_array<mesh::weights_t >(Primitive, varying_structure, "patch_point_weights");

		require_metadata(Primitive, patch_selections, "patch_selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));
		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		k3d::uint_t num_control_points = 0;
		const k3d::uint_t num_patches = patch_selections.size();
		for(k3d::uint_t patch = 0; patch != num_patches; ++patch)
		{
			const k3d::uint_t patch_size = (patch_orders[patch] * (patch_orders[patch] + 1)) / 2;
			num_control_points += patch_size;
			if (patch < num_patches-1 && patch_first_points[patch] + patch_size != patch_first_points[patch+1])
			{
				std::ostringstream buffer;
				buffer << "[" << Primitive.type << "] primitive [patch_first_points[" << (patch+1)
				       << "]] incorrect value [" << patch_first_points[patch+1]
				       << "], expected [" << (patch_first_points[patch] + patch_size) << "]";
				throw std::runtime_error(buffer.str());
			}
		}
		require_table_size(Primitive, varying_structure, "varying", num_control_points);

		require_table_size(Primitive, constant_attributes, "constant", 1);
		require_table_size(Primitive, uniform_attributes, "uniform", uniform_structure.size());
		require_table_size(Primitive, varying_attributes, "varying", uniform_structure.size() * 4);

		return new const_primitive(patch_first_points, patch_orders, patch_selections, patch_materials, patch_points, patch_point_weights, constant_attributes, uniform_attributes, varying_attributes);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "bezier_triangle_patch")
		return 0;

	try
	{
		require_valid_primitive(Primitive);

		table& uniform_structure = require_structure(Primitive, "uniform");
		table& varying_structure = require_structure(Primitive, "varying");

		table& constant_attributes = require_attributes(Primitive, "constant");
		table& uniform_attributes = require_attributes(Primitive, "uniform");
		table& varying_attributes = require_attributes(Primitive, "varying");

		mesh::indices_t& patch_first_points = require_array<mesh::indices_t >(Primitive, uniform_structure, "patch_first_points");
		mesh::orders_t& patch_orders = require_array<mesh::orders_t >(Primitive, uniform_structure, "patch_orders");
		mesh::selection_t& patch_selections = require_array<mesh::selection_t >(Primitive, uniform_structure, "patch_selections");
		mesh::materials_t& patch_materials = require_array<mesh::materials_t >(Primitive, uniform_structure, "patch_materials");
		mesh::indices_t& patch_points = require_array<mesh::indices_t >(Primitive, varying_structure, "patch_points");
		mesh::weights_t& patch_point_weights = require_array<mesh::weights_t >(Primitive, varying_structure, "patch_point_weights");

		require_metadata(Primitive, patch_selections, "patch_selections", metadata::key::selection_component(), string_cast(selection::UNIFORM));
		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		k3d::uint_t num_control_points = 0;
		const k3d::uint_t num_patches = patch_selections.size();
		for(k3d::uint_t patch = 0; patch != num_patches; ++patch)
		{
			const k3d::uint_t patch_size = (patch_orders[patch] * (patch_orders[patch] + 1)) / 2;
			num_control_points += patch_size;
			if (patch < num_patches-1 && patch_first_points[patch] + patch_size != patch_first_points[patch+1])
			{
				std::ostringstream buffer;
				buffer << "[" << Primitive.type << "] primitive [patch_first_points[" << (patch+1)
				       << "]] incorrect value [" << patch_first_points[patch+1]
				       << "], expected [" << (patch_first_points[patch] + patch_size) << "]";
				throw std::runtime_error(buffer.str());
			}
		}
		require_table_size(Primitive, varying_structure, "varying", num_control_points);

		require_table_size(Primitive, constant_attributes, "constant", 1);
		require_table_size(Primitive, uniform_attributes, "uniform", uniform_structure.size());
		require_table_size(Primitive, varying_attributes, "varying", uniform_structure.size() * 4);

		return new primitive(patch_first_points, patch_orders, patch_selections, patch_materials, patch_points, patch_point_weights, constant_attributes, uniform_attributes, varying_attributes);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(pipeline_data<mesh::primitive>& Primitive)
{
  if(!Primitive.get())
    return 0;

	if(Primitive->type != "bezier_triangle_patch")
		return 0;

  return validate(Primitive.writable());
}

} // namespace bezier_triangle_patch

} // namespace k3d

